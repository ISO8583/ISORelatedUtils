/*
	Creative Commons Attribution-NonCommercial-NoDerivs (CC-BY-NC-ND)
	https://creativecommons.org/licenses/by-nc-nd/4.0/
	The most restrictive creative commons license.
	This only allows people to download and share your work for no commercial gain and for no other purposes.
*/


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include "api.h"
#include "db.h"
#include "util.h"
#include "socket.h"
#include "pos/pos.h"

const char * mg_ev_str[] = {
		"MG_EV_ERROR",
		"MG_EV_OPEN",
		"MG_EV_POLL",
		"MG_EV_RESOLVE",
		"MG_EV_CONNECT",
		"MG_EV_ACCEPT",
		"MG_EV_TLS_HS",
		"MG_EV_READ",
		"MG_EV_WRITE",
		"MG_EV_CLOSE",
		"MG_EV_HTTP_MSG",
		"MG_EV_HTTP_CHUNK",
		"MG_EV_WS_OPEN",
		"MG_EV_WS_MSG",
		"MG_EV_WS_CTL",
		"MG_EV_MQTT_CMD",
		"MG_EV_MQTT_MSG",
		"MG_EV_MQTT_OPEN",
		"MG_EV_SNTP_TIME",
		"MG_EV_USER",
};

void mg_log_event(int ev)
{
	char time_str[30];
	if (ev != MG_EV_POLL)
	{
		time_t t = time(0);
		struct tm *time_info = localtime(&t);
		strftime(time_str, sizeof time_str, "%Y/%m/%d %H:%M:%S", time_info);
		printf("[%s.%d] ev: %s\n", time_str, t % 1000, mg_ev_str[ev]);
	}
}

void mg_log_http(struct mg_http_message * http_msg)
{
	printf("<--- response --->\n\t%.*s %.*s %.*s %.*s\n",
			(int) http_msg->method.len, http_msg->method.ptr,
			(int) http_msg->uri.len, http_msg->uri.ptr,
			(int) http_msg->proto.len, http_msg->proto.ptr,
			(int) http_msg->query.len, http_msg->query.ptr);

	struct mg_str k, v, s = http_msg->query;
	printf("<--- params --->\n");

	while (mg_split(&s, &k, &v, '&'))
	{
		printf("\t%.*s -> %.*s\n", (int) k.len, k.ptr, (int) v.len, v.ptr);
	}

	printf("<--- params --->\n");
	printf("<--- headers --->\n");

	for (int i = 0; i < MG_MAX_HTTP_HEADERS; ++i)
	{
		struct mg_http_header *hdr = &http_msg->headers[i];
		if (hdr->name.len)
		{
			printf("\t%.*s -> %.*s\n", (int) hdr->name.len, hdr->name.ptr, (int) hdr->value.len, hdr->value.ptr);
		}
		else
		{
			break;
		}
	}

	printf("<--- headers --->\n");
//	printf("<--- chunk (%ld) --->\n%.*s<--- chunk --->\n", (long) http_msg->chunk.len, (int) http_msg->chunk.len, http_msg->chunk.ptr);
//	printf("<--- payload (%ld) --->\n%.*s<--- payload --->\n", (long) http_msg->body.len, (int) http_msg->body.len, http_msg->body.ptr);
	printf("<--- chunk (%ld) --->\n", (long) http_msg->chunk.len);
	printf("<--- payload (%ld) --->\n", (long) http_msg->body.len);
}

int mg_str_to_int(struct mg_str v)
{
	int res = 0;
	const char *p = v.ptr;

	for (int i = 0, current_is_digit = (v.len > 0 ? isdigit(v.ptr[i]) : 0); i < v.len && current_is_digit; ++i, ++p)
	{
		res *= 10;
		res += v.ptr[i] - '0';
		current_is_digit = isdigit(v.ptr[i]);
	}

	return res < 0 ? 0 : res;
}

int test_api(struct mg_connection *c, struct mg_http_message * http_msg, struct mg_str * caps)
{
	struct mg_str k, v, s = http_msg->body;

	while (mg_split(&s, &k, &v, '&'))
	{
		OsLog(LOG_DEBUG, "<%.*s> -> [%.*s]", (int) k.len, k.ptr, (int) v.len, v.ptr);
	}

	mg_http_reply(c, 200, CONTENT_JSON, "{\"status\": \"ok\"}");
	return 0;
}

struct api_handlers
{
	const char * method;
	const char * pattern;
	request_handler handler;
} api_handlers [] = {
		{"POST",	"/api/v1/test",			test_api},
		{"GET",		"/api/v1/test",			test_api},
		{NULL,		NULL,					NULL},
};

void fn_api (struct mg_connection *c, int ev, void *ev_data, void *fn_data)
{
	static char user[100] = {0};
	static char pass[100] = {0};

	if (ev == MG_EV_POLL)
	{
		return;
	}

	OsLog(LOG_DEBUG, "api: %s", mg_ev_str[ev]);
	if (ev == MG_EV_READ)
	{
		OsLog(LOG_DEBUG, "c->recv.len: %d", c->recv.len);
		if (c->recv.len >= MAX_REQUEST_SIZE)
		{
			OsLog(LOG_DEBUG, "request reaches its limit");
			c->is_closing = 1;
			c->is_draining = 1;
			c->is_full = 1;
		}
	}
	else if (ev == MG_EV_HTTP_MSG && c->is_closing == 0)
	{
		struct mg_str caps[1];
		struct api_handlers * ptr = api_handlers;
		int request_handled = 0;
		struct mg_http_message *http_msg = (struct mg_http_message *) ev_data;

		while (ptr->pattern)
		{
			if (mg_strcmp(http_msg->method, mg_str(ptr->method)) == 0 &&
					mg_match(http_msg->uri, mg_str(ptr->pattern), caps) && ptr->handler != NULL)
			{
				mg_http_creds(http_msg, user, sizeof(user), pass, sizeof(pass));
				int auth_result = -1;
				int retry_count = 0;

				if (strcmp(user, "admin") == 0 &&  strcmp(pass, "pass") == 0)
				{
					auth_result = 0;
				}

				if (auth_result != 0)
				{
					mg_http_reply(c, 401, CONTENT_JSON CONNECTION_CLOSE "WWW-Authenticate: Basic realm=\"POS API Endpoint\"\r\n",
											"{\"error\": \"Not Authorized\"}");
					c->is_draining = 1;
					request_handled = 1;
					break;
				}

				if (mg_strcmp(http_msg->method, mg_str("POST")) == 0)
				{
					struct mg_str * content_type = mg_http_get_header(http_msg, "Content-Type");
					if (content_type == NULL)
					{
						mg_http_reply(c, 400, CONTENT_JSON CONNECTION_CLOSE, "{\"error\": \"Content-Type is not provided\"}");
						c->is_draining = 1;
						request_handled = 1;
						break;
					}
					else if (http_msg->body.len == 0)
					{
						mg_http_reply(c, 400, CONTENT_JSON CONNECTION_CLOSE, "{\"error\": \"Empty body\"}");
						c->is_draining = 1;
						request_handled = 1;
						break;
					}
					else if (mg_strcmp(*content_type, mg_str("application/x-www-form-urlencoded")) != 0)
					{
						mg_http_reply(c, 400, CONTENT_JSON CONNECTION_CLOSE, "{\"error\": \"Content-Type is not valid\"}");
						c->is_draining = 1;
						request_handled = 1;
						break;
					}
				}

				request_handled = 1;
				int request_result = ptr->handler(c, http_msg, caps);
				if (request_result != 0)
				{
					mg_http_reply(c, request_result, CONTENT_JSON CONNECTION_CLOSE, "{\"error\": \"Error\"}");
				}

				break;
			}

			++ptr;
		}

		if (request_handled == 0)
		{
			mg_http_reply(c, 404, CONTENT_JSON CONNECTION_CLOSE, "{\"error\": \"Not Found\"}");
			c->is_draining = 1;
		}
	}
}

void api_server(int * running_flag)
{
	if (running_flag == NULL || *running_flag == 0)
	{
		return;
	}

	struct mg_mgr mgr;
	mg_mgr_init(&mgr);
	mg_http_listen(&mgr, "http://0.0.0.0:8080", fn_api, NULL);

	while (*running_flag)
	{
		pcpos_check_result(&mgr);
		mg_mgr_poll(&mgr, POLL_TIMEOUT);
	}

	mg_mgr_free(&mgr);
}

// -----------------------------------------------------------------------------

void iso_handler(struct mg_connection *c, int ev, void *ev_data, void *fn_data)
{
	switch_connection * swc = (switch_connection *) fn_data;

	if (ev == MG_EV_POLL)
	{
		return;
	}

	OsLog(LOG_DEBUG, "iso: %s", mg_ev_str[ev]);
	if (ev == MG_EV_CLOSE && swc->status != SWC_ERROR)
	{
		swc->status = SWC_DISCONNECT;
	}
	else if (ev == MG_EV_ERROR)
	{
		swc->status = SWC_ERROR;
	}
	else if (ev == MG_EV_CONNECT)
	{
		if (swc->dst->tls)
		{
			mg_tls_init(c, &swc->tls);
		}

		swc->status = SWC_CONNECTED;
	}
	else if (ev == MG_EV_READ && swc->status == SWC_RECEIVING)
	{
		memcpy(swc->recv->buffer + swc->recv->index, c->recv.buf, c->recv.len);
		swc->recv->index += (int) c->recv.len;
		mg_iobuf_del(&c->recv, 0, c->recv.len);

		unsigned char * len = (unsigned char *) swc->recv->buffer;
		if (((len[0] * 256) + len[1] + 2) == swc->recv->index)
		{
			swc->status = SWC_IDLE;
		}
	}
	else if (ev == MG_EV_WRITE && swc->status < SWC_DISCONNECT)
	{
		swc->send_index += *(long *) ev_data;
		if (swc->send_index == swc->recv->index)
		{
			swc->recv->index = 0;
			swc->status = SWC_RECEIVING;
		}
		else
		{
			if (mg_send(swc->c, swc->send->buffer + swc->send_index, swc->send->index - swc->send_index) == 0)
			{
				swc->status = SWC_ERROR;
			}
		}
	}
}

int swc_init(switch_connection * swc)
{
	char connection_string[50];
	if (validate_ip_string(swc->dst->ip, 0) != 0)
	{
		OsLog(LOG_ERR, "%s is not valid ip format", swc->dst->ip);
		return -1;
	}

	sprintf(connection_string, "tcp://%s:%d", swc->dst->ip, swc->dst->port);
	mg_mgr_init(&swc->mgr);
	swc->status = SWC_INIT;
	swc->c = mg_connect(&swc->mgr, connection_string, iso_handler, swc);
	swc->status = swc->c != NULL ? SWC_PRE_CONNECT : SWC_ERROR;

	return swc->status;
}

int swc_wait_connect(switch_connection * swc, int timeout)
{
	while (swc->status != SWC_CONNECTED && swc->status < SWC_DISCONNECT)
	{
		mg_mgr_poll(&swc->mgr, POLL_TIMEOUT);
		--timeout;

		if (timeout == 0)
		{
			swc->status = SWC_TIMEOUT;
			break;
		}
	}

	return swc->status;
}

int swc_send_recv(switch_connection * swc, int timeout)
{
	swc->status = SWC_SENDING;
	swc->send_index = 0;
	int socket_available = 0;

	for (struct mg_connection * c = swc->mgr.conns; c != NULL; c = c->next)
	{
		if (c == swc->c)
		{
			socket_available = 1;
		}
	}

	if (socket_available == 0)
	{
		swc->status = SWC_ERROR;
		return SWC_ERROR;
	}

	if (mg_send(swc->c, swc->send->buffer, swc->send->index) == 0)
	{
		swc->status = SWC_ERROR;
	}

	while (swc->status == SWC_SENDING || swc->status == SWC_RECEIVING)
	{
		mg_mgr_poll(&swc->mgr, POLL_TIMEOUT);
		--timeout;

		if (timeout == 0)
		{
			swc->status = SWC_TIMEOUT;
			break;
		}
	}

	return swc->status;
}

void swc_destroy(switch_connection * swc)
{
	mg_mgr_free(&swc->mgr);
}

void http_response_handler(struct mg_connection *c, int ev, void *ev_data, void *fn_data)
{
	struct http_download_t * dl_spec = (struct http_download_t *) fn_data;
	struct mg_http_message * hm = (struct mg_http_message *) ev_data;

#ifdef DEBUG
	mg_log_event(ev);
#endif

	if (ev == MG_EV_CONNECT)
	{
		if (strncmp(dl_spec->url, "https", 5) == 0)
		{
			mg_tls_init(c, &dl_spec->tls);
		}

		remove(dl_spec->file.path);
		mg_printf(c, "GET %s HTTP/1.0\r\n"
					"Host: %s\r\n"
					"User-Agent: %s\r\n"
					"\r\n",
					mg_url_uri(dl_spec->url), dl_spec->url, dl_spec->agent);
	}
	else if (ev == MG_EV_CLOSE)
	{
		dl_spec->running = 0;
		c->is_draining = 1;
	}
	else if (ev == MG_EV_ERROR)
	{
		dl_spec->error = 1;
		dl_spec->running = 0;
		c->is_draining = 1;
	}
	else if (ev == MG_EV_HTTP_CHUNK)
	{
#ifdef DEBUG
		mg_log_http(hm);
#endif
		if (mg_http_status(hm) == 200)
		{
//			printf("chunk size: %ld\n", hm->chunk.len);
			if (hm->chunk.len == 0)
			{
				dl_spec->running = 0;
				c->is_draining = 1;
			}
			else
			{
				if (dl_spec->export_file)
				{
					if (dl_spec->file.fd == NULL)
					{
						dl_spec->file.fd = fopen(dl_spec->file.path, "a+b");
						if (dl_spec->file.fd == NULL)
						{
							dl_spec->error = 1;
							dl_spec->running = 0;
							c->is_draining = 1;
						}
					}

					if (dl_spec->file.fd != NULL)
					{
						fwrite(hm->chunk.ptr, 1, hm->chunk.len, dl_spec->file.fd);
					}
				}
				else
				{
					if (dl_spec->buffer.size == 0) // dl_spec->buffer.ptr == NULL
					{
						if (hm->body.len != ~0) // -1
						{
							dl_spec->buffer.ptr = malloc(hm->body.len);
							dl_spec->buffer.size = hm->body.len;
						}
						else
						{
							dl_spec->buffer.ptr = malloc(1024);
							dl_spec->buffer.size = 1024;
						}

						if (dl_spec->buffer.ptr == NULL)
						{
							OsLog(LOG_ERROR, "cannot allocate memory");
							dl_spec->error = 1;
							dl_spec->running = 0;
							c->is_draining = 1;
						}
					}
					else if (dl_spec->buffer.used + hm->chunk.len > dl_spec->buffer.size)
					{
						size_t new_size = dl_spec->buffer.size + (hm->chunk.len * 3);
						char * new_ptr = realloc(dl_spec->buffer.ptr, new_size);

						if (new_ptr == NULL)
						{
							OsLog(LOG_ERROR, "cannot reallocate %lu bytes", new_size);

							free(dl_spec->buffer.ptr);
							dl_spec->buffer.size = 0;
							dl_spec->buffer.used = 0;

							dl_spec->error = 1;
							dl_spec->running = 0;
							c->is_draining = 1;
						}
						else
						{
							dl_spec->buffer.size = new_size;
							dl_spec->buffer.ptr = new_ptr;
						}
					}

					if (dl_spec->error == 0)
					{
						memcpy(dl_spec->buffer.ptr + dl_spec->buffer.used, hm->chunk.ptr, hm->chunk.len);
						dl_spec->buffer.used += hm->chunk.len;
					}
				}

				mg_http_delete_chunk(c, hm);
			}
		}
		else
		{
			dl_spec->error = mg_http_status(hm);
			dl_spec->running = 0;
			c->is_draining = 1;
		}
	}
}

int api_download(struct http_download_t * dl_spec)
{
	struct mg_mgr mgr;

	mg_mgr_init(&mgr);
	mg_http_connect(&mgr, dl_spec->url, http_response_handler, dl_spec);

	dl_spec->buffer.ptr = NULL;
	dl_spec->buffer.size = 0;
	dl_spec->buffer.used = 0;
	dl_spec->file.fd = NULL;
	dl_spec->error = 0;
	dl_spec->running = 1;

	while (dl_spec->running != 0)
	{
		mg_mgr_poll(&mgr, POLL_TIMEOUT);
	}

	if (dl_spec->export_file != 0 && dl_spec->file.fd != NULL)
	{
		fclose(dl_spec->file.fd);

		if (dl_spec->error)
		{
			remove(dl_spec->file.path);
		}
	}

	mg_mgr_free(&mgr);
	return dl_spec->error ? -1 : 0;
}
