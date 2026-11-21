#pragma once
#include "http.h"

typedef struct Router Router;

typedef struct Route Route;

Router *createRouter();
int registerRoute(Router *router, HttpMethod method, char *uri,
                  Response *(*handler)(Request *));
void freeRouter(Router *router);

void callRouteHandler(Request *request, Router *router);
