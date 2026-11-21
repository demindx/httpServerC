#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "arena.h"
#include "http.h"
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

bool isCRLFLine(char *line);
void printHeader(Header *header);
int parseRequestLine(MemoryArena *arena, Stream *stream, Request *request);
int parseHeaderLine(MemoryArena *arena, Stream *stream, Header *header,
                    char line[BUFF_SIZE]);

Header *readHeader(MemoryArena *arena, Stream *stream);
HttpMethod getMethod(char *method);

Header *createHeader(MemoryArena *arena, char *name, char *value) {
  Header *header = (Header *)arenaPush(arena, sizeof(Header), false);
  if (header == NULL) {
    return NULL;
  }

  if (name == NULL && value == NULL) {
    return header;
  }

  header->name = arenaStrdup(arena, name);
  if (header->name == NULL) {
    return NULL;
  }

  header->value = arenaStrdup(arena, value);
  if (header->value == NULL) {
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

    if (ptr->name != NULL) {
      free(ptr->name);
    }

    if (ptr->value != NULL) {
      free(ptr->value);
    }

    free(ptr);

    ptr = next;
  }
}

Header *readHeader(MemoryArena *arena, Stream *stream) {
  Header *head = createHeader(arena, NULL, NULL);

  char line[BUFF_SIZE];
  int ok = parseHeaderLine(arena, stream, head, line);

  if (ok != 0) {
    return NULL;
  }

  while (true) {
    Header *header = createHeader(arena, NULL, NULL);

    ok = parseHeaderLine(arena, stream, header, line);

    if (ok != 0) {
      break;
    }

    addHeader(head, header);
  }

  return head;
}

Request *readRequest(MemoryArena *arena, Stream *stream) {
  Request *request = (Request *)arenaPush(arena, sizeof(Request), false);
  if (request == NULL) {
    fprintf(stderr, "ERROR(readRequest): cannot alocate request\n");
    return NULL;
  }

  int status = parseRequestLine(arena, stream, request);
  if (status != 0) {
    fprintf(stderr,
            "ERROR(readRequest): error while parsing the request line: %d\n",
            status);
    return NULL;
  }

  request->header = readHeader(arena, stream);

  char *endptr;
  char *value = getHeaderValue(request->header, "content-length");
  if (value == NULL) {
    return request;
  }

  long contentLength;

  contentLength = strtol(value, &endptr, 10);
  if (endptr == value) {
    fprintf(stderr, "ERROR(Stream): while converting the str to long.\n");
    return NULL;
  }

  request->body =
      (char *)arenaPush(arena, sizeof(char) * (contentLength + 1), false);

  StreamStatus st;
  size_t readed = readFromStream(stream, request->body, contentLength, &st);

  if (st != STREAM_OK) {
    fprintf(stderr, "ERROR(Stream): while reading the body.\n");
  }

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

int parseRequestLine(MemoryArena *arena, Stream *stream, Request *request) {
  char line[BUFF_SIZE];

  if (consumeLine(stream, line, BUFF_SIZE) != STREAM_OK) {
    fprintf(stderr, "ERROR: consumeLine\n");
    return -1;
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

  // strip the CRLF
  version[strlen(version) - 2] = '\0';

  request->method = getMethod(method);
  request->version = arenaStrdup(arena, version);
  request->uri = arenaStrdup(arena, uri);

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

int parseHeaderLine(MemoryArena *arena, Stream *stream, Header *header,
                    char line[BUFF_SIZE]) {
  if (consumeLine(stream, line, BUFF_SIZE) != STREAM_OK) {
    fprintf(stderr, "ERROR: consumeLine\n");
    return -1;
  }

  if (isCRLFLine(line)) {
    fprintf(stderr, "WARNING: CRLF encountered. \n");
    return 1;
  }

  char *name = line;
  char *value = strchr(name, ' ');

  if (value == NULL) {
    fprintf(stderr, "ERROR: get pointer\n");
    return -1;
  }

  *value++ = '\0';

  // strip the CRLF
  value[strlen(value) - 2] = '\0';

  name[strlen(name) - 1] = '\0';

  header->name = arenaStrdup(arena, name);
  header->value = arenaStrdup(arena, value);

  return 0;
}

void printRequest(Request *request) {
  fprintf(stderr, "Request: \n");
  fprintf(stderr, "\turi: %s\n", request->uri);
  fprintf(stderr, "\tversion: %s\n", request->version);

  printHeader(request->header);

  fprintf(stderr, "\tbody: %s\n", request->body);
}

void printHeader(Header *header) {
  fprintf(stderr, "\tHeader: \n");

  Header *ptr = header;
  while (ptr != NULL) {
    fprintf(stderr, "\t\t%s: %s\n", ptr->name, ptr->value);
    ptr = ptr->next;
  }
}

bool isCRLFLine(char *line) {
  if (line[0] == '\r' && line[1] == '\n') {
    return true;
  }

  return false;
}

char *getHeaderValue(Header *head, char *name) {
  Header *ptr = head;

  while (ptr != NULL) {
    if (strcasecmp(ptr->name, name) == 0) {
      return ptr->value;
    }

    ptr = ptr->next;
  }

  return NULL;
}
