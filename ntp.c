/*
	Creative Commons Attribution-NonCommercial-NoDerivs (CC-BY-NC-ND)
	https://creativecommons.org/licenses/by-nc-nd/4.0/
	The most restrictive creative commons license.
	This only allows people to download and share your work for no commercial gain and for no other purposes.
*/


#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>

#include "base.h"

#ifdef WIN32
#include <winsock2.h>
#include <WS2tcpip.h>
#else
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#define NTP_TIMESTAMP_DELTA		2208988800ull

#define LI(packet)				(uint8_t) ((packet.li_vn_mode & 0xC0) >> 6) // (li   & 11 000 000) >> 6
#define VN(packet)				(uint8_t) ((packet.li_vn_mode & 0x38) >> 3) // (vn   & 00 111 000) >> 3
#define MODE(packet)			(uint8_t) ((packet.li_vn_mode & 0x07)) // (mode & 00 000 111) >> 0

typedef struct
{
	uint8_t li_vn_mode;			// Eight bits. li, vn, and mode.
								// li.   Two bits.   Leap indicator.
								// vn.   Three bits. Version number of the protocol.
								// mode. Three bits. Client will pick mode 3 for client.
	uint8_t stratum;			// Eight bits. Stratum level of the local clock.
	uint8_t poll;				// Eight bits. Maximum interval between successive messages.
	uint8_t precision;			// Eight bits. Precision of the local clock.
	uint32_t rootDelay;			// 32 bits. Total round trip delay time.
	uint32_t rootDispersion;	// 32 bits. Max error aloud from primary clock source.
	uint32_t refId;			 	// 32 bits. Reference clock identifier.
	uint32_t refTm_s;			// 32 bits. Reference time-stamp seconds.
	uint32_t refTm_f;			// 32 bits. Reference time-stamp fraction of a second.
	uint32_t origTm_s;			// 32 bits. Originate time-stamp seconds.
	uint32_t origTm_f;			// 32 bits. Originate time-stamp fraction of a second.
	uint32_t rxTm_s;			// 32 bits. Received time-stamp seconds.
	uint32_t rxTm_f;			// 32 bits. Received time-stamp fraction of a second.
	uint32_t txTm_s;			// 32 bits and the most important field the client cares about. Transmit time-stamp seconds.
	uint32_t txTm_f;			// 32 bits. Transmit time-stamp fraction of a second.
} __attribute__((__packed__)) ntp_packet; // Total: 384 bits or 48 bytes.

int resolve_ip(const char * domain, const char * port, int family, int sockettype, int protocol, struct sockaddr_in * out)
{
	struct addrinfo * addr;
	struct addrinfo hint = {0};

	hint.ai_family = family;
	hint.ai_socktype = sockettype;
	hint.ai_protocol = protocol;

	if (getaddrinfo(domain, port, &hint, &addr) == 0)
	{
		struct addrinfo * ptr = addr;
		while (ptr != NULL)
		{
			if (out != NULL)
			{
				memcpy(out, ptr->ai_addr, sizeof(struct sockaddr_in));
				break;
			}

			struct sockaddr_in * adin = ptr->ai_addr;
			printf("ai_flags: %d, ai_family: %d, ai_socktype: %d, ai_protocol: %d, ai_addrlen: %d, "
					"sin_family: %d, sin_port: %d, sin_addr: %u, inet_ntoa: %s\n",
					ptr->ai_flags, ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol, ptr->ai_addrlen,
					adin->sin_family, ntohs(adin->sin_port), adin->sin_addr.s_addr, inet_ntoa(adin->sin_addr));
			ptr = ptr->ai_next;
		}
	}
	else
	{
		return -1;
	}

	freeaddrinfo(addr);
	return 0;
}

int ntp(const char * host, struct tm * out, int timeout, int timezone)
{
	struct sockaddr_in serv_addr = {0};
	ntp_packet packet = {0};
	struct timeval tm = {0};

#ifdef WIN32
	tm.tv_sec = timeout * 1000;
#else
	tm.tv_sec = timeout;
#endif

	// Set the first byte's bits to 00,011,011 for li = 0, vn = 3, and mode = 3. The rest will be left set to zero.
	packet.li_vn_mode = 0x1b; // [00 011 011]

//	struct hostent * server = gethostbyname(host);
//	if (server == NULL)
//	{
//		perror("ERROR, no such host");
//		return -1;
//	}

//	serv_addr.sin_family = AF_INET;
//	serv_addr.sin_port = htons(123);
//	memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);

	if (resolve_ip(host, "123", AF_INET, SOCK_DGRAM, IPPROTO_UDP, &serv_addr) != 0)
	{
		perror("ERROR, no such host");
		return -1;
	}

	int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sockfd < 0)
	{
		perror("ERROR opening socket\n");
		return -1;
	}

	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tm, sizeof(struct tm)) < 0)
	{
		fprintf(stderr, "socket cannot set recv timeout %d\n", errno);
		CLOSE_SOCKET(sockfd);
		return -1;
	}

	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("ERROR connecting\n");
		CLOSE_SOCKET(sockfd);
		return -1;
	}

//	int n = sendto(sockfd, (char*) &packet, sizeof(ntp_packet), 0, &serv_addr, sizeof(serv_addr));
	ssize_t n = send(sockfd, &packet, sizeof(ntp_packet), 0);
	if (n < 0)
	{
		perror("ERROR writing to socket\n");
		CLOSE_SOCKET(sockfd);
		return -1;
	}

//	n = recvfrom(sockfd, &packet, sizeof(ntp_packet), 0, NULL, 0);
	n = recv(sockfd, &packet, sizeof(ntp_packet), 0);
	if (n < 0)
	{
		CLOSE_SOCKET(sockfd);
		return -1;
	}

	CLOSE_SOCKET(sockfd);
	// txTm_s, txTm_f fields contain the time-stamp seconds as the packet left the NTP server.
	// The number of seconds correspond to the seconds passed since 1900.

	// Extract the 32 bits that represent the time-stamp seconds (since NTP epoch) from when the packet left the server.
	// Subtract 70 years worth of seconds from the seconds since 1900.
	// This leaves the seconds since the UNIX epoch of 1970.
	// (1900)------------------(1970)**************************************(Time Packet Left the Server)

	time_t txTm = (time_t) (ntohl(packet.txTm_s) - NTP_TIMESTAMP_DELTA + timezone);
	gmtime_r(&txTm, out);

	return 0;
}