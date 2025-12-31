#ifndef SERVER_H
#define SERVER_H

#include "http.h"
#include <netinet/in.h>

typedef struct HttpServer HttpServer;
typedef struct Connection Connection;

typedef struct Router Router;

HttpServer *createServer(char *addr, int port);
Connection *createConnection(int sockFd, struct sockaddr_in addr);
void setRouter(HttpServer *server, Router *router);
void serve(HttpServer *server);


Router *craeteRouter();
int registerRoute(Router *router, HttpMethod method, char *uri,
                  void (*handler)(Request *));
void freeRouter(Router *router);

#endif
