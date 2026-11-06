#pragma once
#include <stdlib.h>

typedef struct Stream Stream;

Stream *createStream(int fd);

void freeStream(Stream *stream);

int consumeLine(Stream *stream, char *out, size_t out_size);

int fillStream(Stream *stream);

char *readFromStream(Stream *stream, size_t n);

char *readToTheEndOfStream(Stream *stream);
