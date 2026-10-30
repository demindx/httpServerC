#pragma once

#include "http.h"
#include <netinet/in.h>
#include "router.h"

typedef struct HttpServer HttpServer;
typedef struct Connection Connection;


HttpServer *createServer(char *addr, int port);
Connection *createConnection(int sockFd, struct sockaddr_in addr);
void setRouter(HttpServer *server, Router *router);
void serve(HttpServer *server);
