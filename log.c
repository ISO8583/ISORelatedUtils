/*
	Creative Commons Attribution-NonCommercial-NoDerivs (CC-BY-NC-ND)
	https://creativecommons.org/licenses/by-nc-nd/4.0/
	The most restrictive creative commons license.
	This only allows people to download and share your work for no commercial gain and for no other purposes.
*/


#include <log.h>

#include <base.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdarg.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

static const char * level_name[] = {"EMERG", "ALERT", "CRIT", "ERR",
									"WARN", "NOT", "INFO", "DEB"};
static struct
{
	int level;
	int facility;
	int socket_fd;
	struct sockaddr_in serv_addr;
	FILE * file_fd;
	log_callback callback;
	char host_name[50];
	int host_name_len;
} log_config = {
		.level = LOG_INFO,
		.facility = LOG_LOCAL7,
		.socket_fd = -1,
		.serv_addr = {0},
		.file_fd = NULL,
		.callback = NULL,
		.host_name = {0},
		.host_name_len = 0,
};

int log_close_syslog()
{
	if (log_config.socket_fd >= 0)
	{
		CLOSE_SOCKET(log_config.socket_fd);
		log_config.socket_fd = -1;
	}

	return 0;
}

int log_close_file()
{
	if (log_config.file_fd)
	{
		fclose(log_config.file_fd);
		log_config.file_fd = NULL;
	}

	return 0;
}

int log_close()
{
	log_close_file();
	log_close_syslog();
	log_config.callback = NULL;
	return 0;
}

int log_open_syslog()
{
	if (log_config.socket_fd >= 0)
	{
		return 0;
	}

	log_config.socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (log_config.socket_fd < 0)
	{
		perror("socket: cannot create socket");
		return -1;
	}

	int broadcast = 1;
	if (setsockopt(log_config.socket_fd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) != 0)
	{
		perror("setsockopt: cannot set broadcast");
		log_close_syslog();
		return -1;
	}

	log_config.serv_addr.sin_family = AF_INET;
	log_config.serv_addr.sin_addr.s_addr = INADDR_BROADCAST;
	log_config.serv_addr.sin_port = htons(SYSLOG_PORT);

	if (connect(log_config.socket_fd, (struct sockaddr *) &log_config.serv_addr, sizeof(log_config.serv_addr)) != 0)
	{
		perror("connect: cannot connect");
		log_close_syslog();
		return -1;
	}

	return 0;
}

int log_open(int facility, int syslog, const char * restrict path, log_callback callback)
{
	log_config.facility = facility;
	log_close();

	gethostname(log_config.host_name, sizeof(log_config.host_name));
	log_config.host_name_len = strlen(log_config.host_name);
	log_config.host_name[log_config.host_name_len++] = ':';
	log_config.host_name[log_config.host_name_len++] = ' ';

	log_config.callback = callback;

	if (path != NULL)
	{
		log_config.file_fd = fopen(path, "a+");
		if (log_config.file_fd == NULL)
		{
			perror("fopen: cannot open file");
			log_close();
			return -1;
		}
	}

	if (syslog)
	{
		if (log_open_syslog() < 0)
		{
			log_close();
			return -1;
		}
	}

	return 0;
}

int log_set_syslog(const char * restrict ip, int port)
{
	log_open_syslog();
	log_config.serv_addr.sin_addr.s_addr = inet_addr(ip);
	log_config.serv_addr.sin_port = htons(port);
	// todo, test to disable broadcast
	return 0;
}

void log_set_level(int level)
{
	log_config.level = level;
}

int log_(int level, int mode, const char * restrict fmt, ...)
{
	int fmt_len;
	char org_buffer[LOG_BUFFER];
	char syslog_buffer[LOG_BUFFER + FMT_BUFFER];

	if (level > log_config.level)
	{
		return 1;
	}

	va_list args;
	va_start(args, fmt);
	int org_buffer_len = vsnprintf(org_buffer, sizeof(org_buffer), fmt, args);
	va_end(args);

	struct tm now;
	time_t t = time(NULL);
	localtime_r(&t, &now);

	if ((mode & O_SYSLOG) && log_config.socket_fd >= 0)
	{
		fmt_len = sprintf(syslog_buffer, "<%d>", log_config.facility + level);
		fmt_len += strftime(syslog_buffer + fmt_len, sizeof(syslog_buffer) - fmt_len, "%b %d %H:%M:%S ", &now);
		memcpy(syslog_buffer + fmt_len, log_config.host_name, log_config.host_name_len);
		fmt_len += log_config.host_name_len;

		memmove(syslog_buffer + fmt_len, org_buffer, org_buffer_len + fmt_len);

		if (sendto(log_config.socket_fd, syslog_buffer, org_buffer_len + fmt_len, 0,
					&log_config.serv_addr, sizeof(log_config.serv_addr)) < 0)
		{
			perror("sendto: error sending message");
		}
	}

	if ((mode & O_FILE) && log_config.file_fd != NULL)
	{
		fmt_len = sprintf(syslog_buffer, "[%s] ", level_name[level]);
		fmt_len += strftime(syslog_buffer + fmt_len, sizeof(syslog_buffer) - fmt_len, "%Y/%m/%d-%H:%M:%S ", &now);
		fwrite(syslog_buffer, 1, fmt_len, log_config.file_fd);
		fwrite(log_config.host_name, 1, log_config.host_name_len, log_config.file_fd);
		fwrite(org_buffer, 1, org_buffer_len, log_config.file_fd);
		fwrite("\r\n", 1, 2, log_config.file_fd);
		fflush(log_config.file_fd);
	}

	if ((mode & O_FUNC) && log_config.callback != NULL)
	{
		log_config.callback(level, org_buffer);
	}

	return 0;
}
