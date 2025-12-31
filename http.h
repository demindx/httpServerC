#ifndef HTTP_H
#define HTTP_H


#include "status.h"

#define METHODS_LENGTH 9

typedef enum {
  GET,
  POST,
  PUT,
  PATCH,
  DELETE,
  HEAD,
  OPTIONS,
  CONNECT,
  TRACE
} HttpMethod;

extern char *methods[];

typedef struct Header Header;
typedef struct Request Request;
typedef struct Response Response;

Header *createHeader(const char *name, const char *value);
void addHeader(Header *head, Header *header);
void freeHeader(Header *head);

Header *readHeader(int sock);
Request *readRequest(int sock);
Response *createResponse();

#endif
