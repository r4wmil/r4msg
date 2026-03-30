#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <mongoose.h>

#define DB_IMPLEMENTATAION
#include "db.h"
#undef DB_IMPLEMENTATAION

// HTTP

void http_reply_binary(struct mg_connection* c, uint8_t* buf, size_t len) {
	mg_printf(c,
			"HTTP/1.1 200 OK\r\n"
			"Connection: close\r\n"
			"Content-Type: application/octet-stream\r\n"
			"Content-Length: %d\r\n\r\n",
			len);
	mg_send(c, buf, len);
}

void h_http_binary(struct mg_connection* c, struct mg_str data) {
	mg_hexdump(data.buf, data.len);
	uint8_t err = 0;
	uint16_t msg_type = 0;
	struct mg_str username = {0};
	struct mg_str password = {0};
	BS_READ(&data, 2, &msg_type, &err);
	BS_READ(&data, 4, &username.len, &err);
	BS_READ_SLICE(&data, username.len, &username.buf, &err);
	BS_READ(&data, 4, &password.len, &err);
	BS_READ_SLICE(&data, password.len, &password.buf, &err);
	if (err == 1) { goto defer; }
	if (username.len > 24) { err = 2; goto defer; }
	for (size_t i = 0; i < username.len; i++) {
		char c = username.buf[i];
		if (c >= 'a' && c <= 'z') { continue; }
		if (c >= 'A' && c <= 'Z') { continue; }
		if (c >= '0' && c <= '9') { continue; }
		err = 3; goto defer;
	}
	DBUserAdd(username, password);
	MG_INFO(("username='%.*s' password='%.*s'\n",
				(int)username.len,
				username.buf,
				(int)password.len,
				password.buf));
defer:
	http_reply_binary(c, &(uint8_t){err}, 1);
}

void h_http_msg(struct mg_connection* c, void* ev_data) {
	struct mg_http_message* hm = (struct mg_http_message*)ev_data;
	if (!mg_strcmp(hm->method, mg_str("POST"))) {
		h_http_binary(c, hm->body);
		return;
	}
	struct mg_http_serve_opts opts = { .root_dir = "./web" };
	mg_http_serve_dir(c, hm, &opts);
}

void h_event(struct mg_connection* c, int ev, void* ev_data) {
	switch (ev) {
		case MG_EV_HTTP_MSG:
			h_http_msg(c, ev_data);
			break;
		case MG_EV_CLOSE:
			break;
	}
}

bool mainloop_flag = true;
void mainloop_break(int sig) { mainloop_flag = false; }

int main() {
	mg_log_set(MG_LL_INFO);

	struct mg_mgr mgr;
	mg_mgr_init(&mgr);

	char addrstr[32];
	snprintf(addrstr, sizeof(addrstr), "http://0.0.0.0:%d", 6969);
	mg_http_listen(&mgr, addrstr, h_event, NULL);
	DBInit();

	signal(SIGINT, mainloop_break);
	signal(SIGTERM, mainloop_break);
	while (mainloop_flag) {
		mg_mgr_poll(&mgr, 1000);
	}

	DBDeinit();
	mg_mgr_free(&mgr);
	printf("Closed.\n");
	return 0;
}
