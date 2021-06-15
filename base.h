/*
	Creative Commons Attribution-NonCommercial-NoDerivs (CC-BY-NC-ND)
	https://creativecommons.org/licenses/by-nc-nd/4.0/
	The most restrictive creative commons license.
	This only allows people to download and share your work for no commercial gain and for no other purposes.
*/

#ifndef PAX_BASE_H
#define PAX_BASE_H 1

#ifdef WIN32
#define CLOSE_SOCKET(x)				closesocket((x))
#define gmtime_r(...)				gmtime_s(__VA_ARGS__)
#define localtime_r(...)			localtime_s(__VA_ARGS__)
#else
#define CLOSE_SOCKET(x)				close((x))
#endif

#endif //PAX_BASE_H