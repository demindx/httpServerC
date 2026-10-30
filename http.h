#pragma once

#include "status.h"
#include "stream.h"

#define METHODS_LENGTH 10

typedef enum {
  GET,
  POST,
  PUT,
  PATCH,
  DELETE,
  HEAD,
  OPTIONS,
  CONNECT,
  TRACE,
  QUERY
} HttpMethod;

extern char *methods[];

typedef struct Header Header;

typedef struct Response Response;

typedef struct {
  HttpMethod method;
  char *uri;
  char *version;

  Header *header;

  char *body;
} Request;

Header *createHeader(const char *name, const char *value);
void addHeader(Header *head, Header *header);
void freeHeader(Header *head);

Request *readRequest(Stream *stream);
Response *createResponse();
