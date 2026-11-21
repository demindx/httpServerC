#pragma once

#include <netinet/in.h>
#include "router.h"

typedef struct HttpServer HttpServer;
// typedef struct Connection Connection;


HttpServer *createServer(char *addr, int port);
void freeServer(HttpServer *server);
// Connection *createConnection(int sockFd, struct sockaddr_in addr);
void setRouter(HttpServer *server, Router *router);
void serve(HttpServer *server);
