#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "http.h"
#include "status.h"
#include "stream.h"

#define BUFF_SIZE 8192

char *methods[] = {"GET",  "POST",    "PUT",     "PATCH", "DELETE",
                   "HEAD", "OPTIONS", "CONNECT", "TRACE", "QUERY"};

struct Header {
  char *name;
  char *value;

  Header *next;
  Header *prev;
};

struct Response {
  char *version;
  HttpStatus status;

  char *body;
};

Header *readHeader(Stream *stream);
int parseRequestLine(Stream *stream, Request *request);
HttpMethod getMethod(char *method);

Header *createHeader(const char *name, const char *value) {
  Header *header = malloc(sizeof(Header));
  if (header == NULL) {
    return NULL;
  }

  if (name == NULL && value == NULL) {
    return header;
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
  Header *ptr = head;

  while (ptr != NULL) {
    Header *next = ptr->next;

    free(ptr->name);
    free(ptr->value);
    free(ptr);

    ptr = next;
  }
}

Header *readHeader(Stream *stream) {
  Header *header = createHeader(NULL, NULL);
  char buff[BUFF_SIZE];

  return NULL;
}

Request *readRequest(Stream *stream) {
  Request *request = malloc(sizeof(Request));
  if (request == NULL) {
		fprintf(stderr, "ERROR(readRequest): cannot alocate request");
    return NULL;
  }

	int status = parseRequestLine(stream, request);
  if ( status != 0) {
		fprintf(stderr, "ERROR(readRequest): error while parsing the request line: %d", status);
    free(request);
    return NULL;
  }

  request->header = readHeader(stream);
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

int parseRequestLine(Stream *stream, Request *request) {
  char line[BUFF_SIZE];

  for (;;) {
    int c = consumeLine(stream, line, BUFF_SIZE);

    if (c > 0) {
      break;
    }

    if (c < 0) {
      return -1;
    }

    int f = fillStream(stream);

    if (f != 1) {
      return -1;
    }
  }

  char *method = line;
  char *uri = strchr(line, ' ');
  if (uri == NULL) {
    return -1;
  }
  *uri++ = '\0';

  char *version = strchr(uri, ' ');
  if (version == NULL) {
    return -1;
  }
  *version++ = '\0';

	request->method = getMethod(method);
	request->version = strdup(version);
	request->uri = strdup(uri);

  return 0;
}

HttpMethod getMethod(char *method) {
  for (int i = 0; i <= METHODS_LENGTH; ++i) {
    if (strcmp(method, methods[i]) == 0) {
      return i;
    }
  }

  return -1;
}
