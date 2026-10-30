#pragma once
#include "http.h"

typedef struct Router Router;

Router *createRouter();
int registerRoute(Router *router, HttpMethod method, char *uri,
                  void (*handler)(Request *));
void freeRouter(Router *router);
