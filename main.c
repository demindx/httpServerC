#include "http.h"
#include "server.h"
#include <stdio.h>

void test_handler(Request *request) {
	printf("Hello from handler\n");
	fflush(stdout);
}

int main() {
	HttpServer *server = createServer("127.0.0.1", 8080);

	serve(server);
	// Router *router = craeteRouter();
	//
	// printf("%s\n", methods[0]);
	// registerRoute(router, GET, "/test", test_handler);
	//
	// freeRouter(router);
}
