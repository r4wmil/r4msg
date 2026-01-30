#include <stdio.h>

#include <mongoose.h>

void fn(struct mg_connection* c, int ev, void* ev_data) {
	switch (ev) {
		case MG_EV_HTTP_MSG:
			struct mg_http_message* hm = (struct mg_http_message*)ev_data;
			struct mg_http_serve_opts opts = { .root_dir = "./web" };
			mg_http_serve_dir(c, hm, &opts);
			return;
			break;
		case MG_EV_CLOSE:
			break;
	}
}

int main() {
	struct mg_mgr mgr;
	mg_mgr_init(&mgr);

	char addrstr[32];
	snprintf(addrstr, sizeof(addrstr), "http://0.0.0.0:%d", 6969);
	mg_http_listen(&mgr, addrstr, fn, NULL);

	for (;;) {
		mg_mgr_poll(&mgr, 1000);
	}

	mg_mgr_free(&mgr);
	return 0;
}
