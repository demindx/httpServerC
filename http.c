#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "http.h"
#include "status.h"

#define BUFF_SIZE 8192

char *methods[] = {"GET",  "POST",    "PUT",     "PATCH", "DELETE",
                   "HEAD", "OPTIONS", "CONNECT", "TRACE"};

struct Header {
  char *name;
  char *value;

  Header *next;
  Header *prev;
};

struct Request {
  HttpMethod method;
  char *uri;
  char *version;

  Header *header;

  char *body;
};

struct Response {
  char *version;
  HttpStatus status;

  char *body;
};

Header *createHeader(const char *name, const char *value) {
  Header *header = malloc(sizeof(Header));
  if (header == NULL) {
    return NULL;
  }

  header->name = strdup(name);
  if (header->name == NULL) {
    free(header);
    return NULL;
  }

  header->value = strdup(value);
  if (header->value == NULL) {
    free(header->name);
    free(header);
    return NULL;
  }

  header->next = NULL;
  header->prev = NULL;

  return header;
}

void addHeader(Header *head, Header *header) {
  if (head == NULL || header == NULL) {
    return;
  }

  Header *ptr = head;
  while (ptr->next != NULL) {
    ptr = ptr->next;
  }

  ptr->next = header;
}

void freeHeader(Header *head) {
  if (head == NULL) {
    return;
  }

  head->next = NULL;
  head->prev = NULL;
  free(head->name);
  free(head->value);

  free(head);
}

Header *readHeader(int sock) {
  // TODO: read header

  return NULL;
}

Request *readRequest(int sock) {
  Request *request = malloc(sizeof(Request));
  if (request == NULL) {
    return NULL;
  }

  request->header = readHeader(sock);

  return request;
}

int parseHttpMethod(char *buff) {
  for (int i = 0; i < METHODS_LENGTH; ++i) {
    if (strcmp(buff, methods[i]) == 0) {
      return i;
    }
  }

  return -1;
}

int parseRequestLine(int sock, Request *request) {
  // TODO: parse request line

  return -1;
}
