#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "http.h"
#include "server.h"

struct HttpServer {
  int socket;
  struct sockaddr_in addr;
  Router *router;
};

struct Route {
  HttpMethod method;
  char *uri;
  void (*handler)(Request *);
  struct Route *next;
};

struct Router {
  struct Route *routes;
};

struct Route *createRoute(HttpMethod method, char *uri,
                          void (*handler)(Request *)) {
  if (uri == NULL) {
    fprintf(stderr, "Incorrect uri\n");
    return NULL;
  }

  if (handler == NULL) {
    fprintf(stderr, "Handler is NULL\n");
    return NULL;
  }

  struct Route *route = (struct Route *)malloc(sizeof(struct Route));

  if (route == NULL) {
    perror("Error while allocating route: ");
    return NULL;
  }

  route->uri = strdup(uri);
  if (route->uri == NULL) {
    perror("Error while copy uri to route: ");
    free(route);
    return NULL;
  }

  route->handler = handler;

  return route;
}

Router *craeteRouter() {
  Router *router = (Router *)malloc(sizeof(Router));
  if (router == NULL) {
    perror("Error while allocating Router: ");
    return NULL;
  }

  router->routes = NULL;

  return router;
}

int registerRoute(Router *router, HttpMethod method, char *uri,
                  void (*handler)(Request *)) {
  if (router == NULL) {
    fprintf(stderr, "Router cannot be NULL\n");
    return 0;
  }

  if (uri == NULL) {
    fprintf(stderr, "Incorrect uri\n");
    return 0;
  }

  if (handler == NULL) {
    fprintf(stderr, "Handler is NULL\n");
    return 0;
  }

  if (router->routes == NULL) {
    router->routes = createRoute(method, uri, handler);

    return 0;
  }

  struct Route *ptr = router->routes;

  while (ptr->next != NULL) {
    ptr = ptr->next;
  }

  ptr->next = createRoute(method, uri, handler);

  return 1;
}

HttpServer *createServer(char *addr, int port) {
  if (addr == NULL) {
    fprintf(stderr, "Incorrect address\n");
    return NULL;
  }

  if (port < 1024 || port > 65535) {
    fprintf(stderr, "Incorrect port\n");
    return NULL;
  }

  HttpServer *server = (HttpServer *)malloc(sizeof(HttpServer));
  if (server == NULL) {
    perror("Error while allocating HttpServer: ");
    return NULL;
  }

  if ((server->socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("Error while creating socket: ");
    free(server);

    return NULL;
  }

  server->addr.sin_family = AF_INET;
  server->addr.sin_port = htons(port);
  server->addr.sin_addr.s_addr = inet_addr(addr);

  if (bind(server->socket, (struct sockaddr *)&server->addr,
           sizeof(server->addr)) != 0) {
    perror("Error while binding socket: ");

    return NULL;
  }

  return server;
}

void setRouter(HttpServer *server, Router *router) {
  if (router == NULL) {
    return;
  }

  server->router = router;
}

void freeRouter(Router *router) {
  struct Route *route = router->routes;

  while (route != NULL) {
    struct Route *tmp = route;
    route = route->next;
    free(tmp->uri);
    free(tmp);
  }

  free(router);
}

void serve(HttpServer *server) {
  if (listen(server->socket, 5) != 0) {
    perror("Error while trying to listen socket: ");
    return;
  }

  int client_fd;
  struct sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  memset(&client_addr, 0, sizeof(client_addr));

  while (1) {
    if ((client_fd = accept(server->socket, (struct sockaddr *)&client_addr,
                            &client_addr_len)) < 0) {
      perror("Error while accepting connection: ");
      break;
    }

    char buff[30000];

    recv(client_fd, &buff, 30000, 0);

    printf("%s", buff);
  }
}
