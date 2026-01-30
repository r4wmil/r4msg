#include <stdio.h>
#include <string.h>

#include <mongoose.h>

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
	uint8_t result = 0;
	mg_hexdump(&result, 1);
	http_reply_binary(c, &result, 1);
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

int main() {
	mg_log_set(MG_LL_INFO);

	struct mg_mgr mgr;
	mg_mgr_init(&mgr);

	char addrstr[32];
	snprintf(addrstr, sizeof(addrstr), "http://0.0.0.0:%d", 6969);
	mg_http_listen(&mgr, addrstr, h_event, NULL);

	for (;;) {
		mg_mgr_poll(&mgr, 1000);
	}

	mg_mgr_free(&mgr);
	return 0;
}
