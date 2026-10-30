#pragma once
#include "http.h"

typedef struct Router Router;

typedef struct Route{
  HttpMethod method;
  char *uri;
  void (*handler)(Request *);
  struct Route *next;
} Route ;


Router *createRouter();
int registerRoute(Router *router, HttpMethod method, char *uri,
                  void (*handler)(Request *));
void freeRouter(Router *router);
Route *getRouteByUri(Router * router, char *uri);
