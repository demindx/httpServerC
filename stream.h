#pragma once
#include <stdlib.h>

typedef struct Stream Stream;

Stream *createStream(int fd);
void destroyStream(Stream *stream);

int consumeLine(Stream *stream, char *out, size_t out_size);

int fillStream(Stream *stream);
