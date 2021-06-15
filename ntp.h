/*
	Creative Commons Attribution-NonCommercial-NoDerivs (CC-BY-NC-ND)
	https://creativecommons.org/licenses/by-nc-nd/4.0/
	The most restrictive creative commons license.
	This only allows people to download and share your work for no commercial gain and for no other purposes.
*/

#ifndef _NTP_H_
#define _NTP_H_ 1

#include <time.h>

int resolve_ip(const char * domain, const char * port, int family, int sockettype, int protocol, struct sockaddr_in * out);
int ntp(const char * host, struct tm * out, int timeout, int timezone);

#endif // _NTP_H_
