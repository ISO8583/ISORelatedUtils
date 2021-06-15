/*
	Creative Commons Attribution-NonCommercial-NoDerivs (CC-BY-NC-ND)
	https://creativecommons.org/licenses/by-nc-nd/4.0/
	The most restrictive creative commons license.
	This only allows people to download and share your work for no commercial gain and for no other purposes.
*/


#ifndef _LOG_H_
#define _LOG_H_ 1

#include <syslog.h>

#define SYSLOG_PORT				514
#define FMT_BUFFER				512
#define LOG_BUFFER				2048

typedef void (* log_callback)(int level, const char * log_buffer);

enum
{
	O_SYSLOG = 1 << 0,
	O_FILE = 1 << 1,
	O_FUNC = 1 << 2,
};

int log_open(int facility, int syslog, const char * restrict path, log_callback callback);
int log_set_syslog(const char * restrict ip, int port);
void log_set_level(int level);
int log_(int level, int mode, const char * restrict fmt, ...);
int log_close();

#endif //_LOG_H_
