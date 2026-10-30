#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "router.h"

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

Router *createRouter() {
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
