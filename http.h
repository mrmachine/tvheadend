/*
 *  tvheadend, HTTP interface
 *  Copyright (C) 2007 Andreas �man
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef HTTP_H_
#define HTTP_H_

#include "tcp.h"

LIST_HEAD(rtsp_session_head, rtsp_session);

#define http_printf(x, fmt...) tcp_printf(&(x)->hc_tcp_session, fmt)

LIST_HEAD(http_arg_list, http_arg);

typedef struct http_arg {
  LIST_ENTRY(http_arg) link;
  char *key;
  char *val;
} http_arg_t;


typedef struct http_connection {
  tcp_session_t hc_tcp_session; /* Must be first */
  char *hc_url;
  int hc_keep_alive;

  struct http_arg_list hc_args;

  enum {
    HTTP_CON_WAIT_REQUEST,
    HTTP_CON_READ_HEADER,
    HTTP_CON_END,
  } hc_state;

  enum {
    HTTP_CMD_GET,
    RTSP_CMD_DESCRIBE,
    RTSP_CMD_OPTIONS,
    RTSP_CMD_SETUP,
    RTSP_CMD_TEARDOWN,
    RTSP_CMD_PLAY,
    RTSP_CMD_PAUSE,
  } hc_cmd;

  enum {
    HTTP_VERSION_0_9,
    HTTP_VERSION_1_0,
    HTTP_VERSION_1_1,
    RTSP_VERSION_1_0,
  } hc_version;

  char *hc_username;
  char *hc_password;

  struct rtsp_session_head hc_rtsp_sessions;

} http_connection_t;



void http_start(void);

void http_arg_flush(struct http_arg_list *list);

char *http_arg_get(struct http_arg_list *list, char *name);

void http_arg_set(struct http_arg_list *list, char *key, char *val);

int http_tokenize(char *buf, char **vec, int vecsize, int delimiter);

void http_error(http_connection_t *hc, int error);


typedef int (http_callback_t)(http_connection_t *hc, const char *remain,
			      void *opaque);

typedef struct http_path {
  LIST_ENTRY(http_path) hp_link;
  const char *hp_path;
  void *hp_opaque;
  http_callback_t *hp_callback;
  int hp_len;
} http_path_t;

http_path_t *http_path_add(const char *path, void *opaque,
			   http_callback_t *callback);

#endif /* HTTP_H_ */