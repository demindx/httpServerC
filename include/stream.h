#pragma once
#include "arena.h"
#include <stdlib.h>

typedef enum {
  STREAM_OK = 1,
  STREAM_EOF = 0,
  STREAM_ERROR = -1,
  STREAM_TIMEOUT = -2,
  STREAM_TOO_LONG = -3,
} StreamStatus;

typedef struct Stream Stream;

Stream *createStream(MemoryArena *arena, int fd);

int consumeLine(Stream *stream, char *out, size_t out_size);

size_t readFromStream(Stream *stream, char *out, size_t n, StreamStatus *st);

char *readToTheEndOfStream(Stream *stream);
