#pragma once

#include "arena.h"
#include "status.h"
#include "stream.h"

#define METHODS_LENGTH 10

typedef enum {
  GET = 0,
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

typedef struct Response {
  char *version;
  HttpStatus status;

  Header *header;

  char *body;
} Response;

typedef struct {
  HttpMethod method;
  char *uri;
  char *version;

  Header *header;

  char *body;
} Request;

Header *createHeader(MemoryArena *arena, char *name, char *value);
char *getHeaderValue(Header *head, char *name);

void addHeader(Header *head, Header *header);

Request *readRequest(MemoryArena *arena, Stream *stream);
Response *createResponse(MemoryArena *arena);

void printRequest(Request *request);
