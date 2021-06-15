/*
	Creative Commons Attribution-NonCommercial-NoDerivs (CC-BY-NC-ND)
	https://creativecommons.org/licenses/by-nc-nd/4.0/
	The most restrictive creative commons license.
	This only allows people to download and share your work for no commercial gain and for no other purposes.
*/


#ifndef SOCKET_H
#define SOCKET_H 1

#ifdef WIN32
int socket_init_win32();
void socket_deinit_win32();
#else
#include <net/if.h>
#endif

int socket_init(char * ip, int port, int timeout);
int socket_close(int fd);
int socket_send(int fd, char * in, int len);
int socket_recv(int fd, char * out, int * len);
int socket_if_stat(const char * ifname, char * out_mac, char * out_ip, int * out);
int socket_check_up_down(const char * ifname);

#endif // SOCKET_H
