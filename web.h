/*
	Creative Commons Attribution-NonCommercial-NoDerivs (CC-BY-NC-ND)
	https://creativecommons.org/licenses/by-nc-nd/4.0/
	The most restrictive creative commons license.
	This only allows people to download and share your work for no commercial gain and for no other purposes.
*/


#ifndef _API_H
#define _API_H 1
#include <mongoose.h>

#define MAX_REQUEST_SIZE			2048

struct http_download_t {
	// must be assigned
	const char * agent;
	const char * url;
	int export_file;
	struct mg_tls_opts tls;
	struct {
		const char * path;
		FILE * fd;
	} file;
	struct {
		char * ptr;
		size_t size;
		size_t used;
	} buffer;
	int running;
	int error;
};

extern const char * mg_ev_str[];
extern void mg_log_event(int ev);

int api_download(struct http_download_t * dl_spec);

#endif //_API_H