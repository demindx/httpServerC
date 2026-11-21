#include "http.h"
#include "server.h"
#include <stdio.h>

Response *test_handler(Request *request) {
	printf("Hello from handler\n");
	fflush(stdout);

	return NULL;
}


int main() {
	HttpServer *server = createServer("127.0.0.1", 8082);
	if (server == NULL) {
		fprintf(stderr, "Failed to create server\n");
		return 1;
	}

	Router *router = createRouter();
	if (router == NULL) {
		fprintf(stderr, "Failed to create router\n");
		return 1;
	}

	registerRoute(router, GET, "/test", test_handler);
	setRouter(server, router);

	serve(server);

	freeServer(server);
	return 0;
}
