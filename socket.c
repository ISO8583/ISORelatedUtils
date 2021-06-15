/*
	Creative Commons Attribution-NonCommercial-NoDerivs (CC-BY-NC-ND)
	https://creativecommons.org/licenses/by-nc-nd/4.0/
	The most restrictive creative commons license.
	This only allows people to download and share your work for no commercial gain and for no other purposes.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>

#include "socket.h"

#include <base.h>

#ifdef WIN32
#include <winsock2.h>

static WSADATA wsaData;

int socket_init_win32()
{
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);

	if (iResult != 0)
	{
		printf("WSAStartup failed with error: %d\n", iResult);
		return 0;
	}

	return -1;
}

void socket_deinit_win32()
{
	WSACleanup();
}

#else

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>

#endif

#ifdef WIN32

int socket_init(char * ip, int port, int timeout)
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);

	if (fd < 0)
	{
		printf("socket cannot create: %d\n", WSAGetLastError());
	}

	struct sockaddr_in server;
	server.sin_addr.s_addr = inet_addr(ip);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);

	if (connect(fd , (struct sockaddr *) & server, sizeof(server)) < 0)
	{
		printf("socket cannot connect %d\n", WSAGetLastError());
		CLOSE_SOCKET(fd);
		return -1;
	}

	struct timeval tm;
	tm.tv_sec = timeout * 1000;
	tm.tv_usec = 0;

	if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tm, sizeof(struct tm)) < 0)
	{
		printf("socket cannot set recv timeout %d\n", errno);
		CLOSE_SOCKET(fd);
		return -1;
	}

	if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tm, sizeof tm) < 0)
	{
		printf("socket cannot set send timeout %d\n", errno);
		CLOSE_SOCKET(fd);
		return -1;
	}

	return fd;
}

#else
int socket_init(char * ip, int port, int timeout)
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);

	if (fd < 0)
	{
		printf("socket cannot create: %d\n", errno);
		return -1;
	}

	struct sockaddr_in server;
	server.sin_addr.s_addr = inet_addr(ip);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);

	long arg;
	if ((arg = fcntl(fd, F_GETFL, NULL)) < 0)
	{
		printf("Error fcntl(..., F_GETFL) (%s)\n", strerror(errno));
		CLOSE_SOCKET(fd);
		return -1;
	}

	arg |= O_NONBLOCK;

	if ((arg = fcntl(fd, F_SETFL, arg)) < 0)
	{
		printf("Error fcntl(..., F_SETFL) (%s)\n", strerror(errno));
		CLOSE_SOCKET(fd);
		return -1;
	}

	if (connect(fd , (struct sockaddr *) & server, sizeof(server)) < 0)
	{
		if (errno == EINPROGRESS)
		{
			int valopt;
			socklen_t lon;
			struct timeval tv;

			do {
				tv.tv_sec = 15;
				tv.tv_usec = 0;

				fd_set myset;
				FD_ZERO(&myset);
				FD_SET(fd, &myset);

				int res = select(fd+1, NULL, &myset, NULL, &tv);
				if (res < 0 && errno != EINTR)
				{
					printf("Error connecting %d - %s\n", errno, strerror(errno));
					CLOSE_SOCKET(fd);
					return -1;
				}
				else if (res > 0)
				{
					// Socket selected for write
					lon = sizeof(int);
					if (getsockopt(fd, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon) < 0)
					{
						printf("Error in getsockopt() %d - %s\n", errno, strerror(errno));
						CLOSE_SOCKET(fd);
						return -1;
					}
					// Check the value returned...
					if (valopt)
					{
						printf("Error in delayed connection() %d - %s\n", valopt, strerror(valopt));
						CLOSE_SOCKET(fd);
						return -1;
					}

					break;
				}
				else
				{
					printf("Timeout in select() - Canceling!\n");
					CLOSE_SOCKET(fd);
					return -1;
				}
			} while (1);
		}
		else
		{
			printf("socket cannot connect %d %s\n", errno, strerror(errno));
			CLOSE_SOCKET(fd);
			return -1;
		}
	}

	if ((arg = fcntl(fd, F_GETFL, NULL)) < 0)
	{
		fprintf(stderr, "Error fcntl(..., F_GETFL) (%s)\n", strerror(errno));
		CLOSE_SOCKET(fd);
		return -1;
	}

	arg &= (~O_NONBLOCK);

	if (fcntl(fd, F_SETFL, arg) < 0) {
		fprintf(stderr, "Error fcntl(..., F_SETFL) (%s)\n", strerror(errno));
		CLOSE_SOCKET(fd);
		return -1;
	}

	struct timeval tm;
	tm.tv_sec = timeout;
	tm.tv_usec = 0;

	if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tm, sizeof(struct tm)) < 0)
	{
		printf("socket cannot set recv timeout %d\n", errno);
		CLOSE_SOCKET(fd);
		return -1;
	}

	if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tm, sizeof tm) < 0)
	{
		printf("socket cannot set send timeout %d\n", errno);
		CLOSE_SOCKET(fd);
		return -1;
	}

	return fd;
}

#endif

int socket_close(int fd)
{
	int res;

#ifdef WIN32
	res = shutdown(fd, SD_BOTH);
	if (res != 0)
	{
		printf("socket shutdown: %d\n", WSAGetLastError());
	}
#endif

	res = CLOSE_SOCKET(fd);
	if (res != 0)
	{
		printf("socket: %d %s\n", errno, strerror(errno));
	}

	return res;
}

int socket_send(int fd, char * in, int len)
{
	int s = 0;
	int res;
	while (1)
	{
		res = send(fd, in + s, len - s, 0);
		if (res < 0)
		{
#ifdef WIN32
			printf("socket send %d\n", WSAGetLastError());
#else
			printf("socket send %d %s\n", errno, strerror(errno));
#endif
			return -1;
		}
		else if (res > 0)
		{
			s += res;
		}
		else
		{
			return s;
		}

		if (s == len)
		{
			return s;
		}
	}

	return -1;
}

int socket_recv(int fd, char * out, int * len)
{
	char temp[100];
	int t = 0;
	ssize_t i_out = 0;
	ssize_t res;

	do {
		res = recv(fd, temp, sizeof(temp), 0);
		if (res > 0)
		{
			memcpy(out + i_out, temp, res);
			i_out += res;

			t = (((unsigned char) out[0]) * 256) + ((unsigned char) out[1]);
			if (i_out > 2 && t == (i_out - 2))
			{
				break;
			}
		}
		else if (res == 0)
		{
			printf("socket recv closed %d %s\n", errno, strerror(errno));
		}
		else
		{
#ifdef WIN32
			printf("socket recv %d\n", WSAGetLastError());
#else
			printf("socket recv %d %s\n", errno, strerror(errno));
#endif
			break;
		}
	} while (res > 0);

	if (i_out > 2 && t == (i_out - 2))
	{
		*len = (int) i_out;
		return i_out;
	}
	else
	{
		return -1;
	}
}

int socket_if_stat(const char * ifname, char * out_mac, char * out_ip, int * out_stat)
{
#ifndef WIN32
	struct ifreq ifr;
	int res = 0;
	int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

	if (fd < 0)
	{
		res = -1;
	}
	else
	{
		strcpy(ifr.ifr_name, ifname);

		if (out_stat != NULL)
		{
			if (ioctl(fd, SIOCGIFFLAGS, &ifr) == 0)
			{
				*out_stat = ifr.ifr_ifru.ifru_flags;
			}
			else
			{
				res = -1;
			}
		}

		if (out_mac != NULL)
		{
			if (ioctl(fd, SIOCGIFHWADDR, &ifr) == 0)
			{
				char * m = ifr.ifr_addr.sa_data;
				sprintf(out_mac, "%02X:%02X:%02X:%02X:%02X:%02X", m[0], m[1], m[2], m[3], m[4], m[5]);
			}
			else
			{
				res = -1;
			}
		}

		if (out_ip)
		{
			ifr.ifr_addr.sa_family = AF_INET;
			if (ioctl(fd, SIOCGIFADDR, &ifr) == 0)
			{
				strcpy(out_ip, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
			}
			else
			{
				res = -1;
			}
		}

		close(fd);
	}

	return res;
#else
	return -1;
#endif
}

int socket_check_up_down(const char * ifname)
{
#ifndef WIN32
	int eth_up = 0;
	if (socket_if_stat(ifname, NULL, NULL, &eth_up) == 0)
	{
		return (eth_up & (IFF_UP | IFF_RUNNING)) == (IFF_UP | IFF_RUNNING);
	}
	else
	{
		return -1;
	}
#else
	return -1;
#endif
}