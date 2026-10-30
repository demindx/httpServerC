#include <asm-generic/errno-base.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "stream.h"

#define BUFF_SIZE 8192

struct Stream {
  int fd;

  char buff[BUFF_SIZE];

  size_t len;
};

Stream *createStream(int fd) {

  Stream *stream = (Stream *)malloc(sizeof(Stream));

  if (stream == NULL) {
    fprintf(stderr, "ERROR(Stream): failed to alocate the stream");
    return NULL;
  }

  stream->fd = fd;
  stream->len = 0;
  stream->buff[stream->len] = '\0';

  return stream;
}

void destroyStream(Stream *stream) { free(stream); }

/*
 * return -1 if no space left in buff ( needs to do consuming line)
 * 1 if is filled
 * 0 nothing readed
 * */
int fillStream(Stream *stream) {
  if (stream->len >= sizeof(stream->buff) - 1) {
    fprintf(stderr, "ERROR(Stream): no space left in buff");
    errno = ENOSPC;
    return -1;
  }

  for (;;) {
    ssize_t r = recv(stream->fd, stream->buff + stream->len,
                     sizeof(stream->buff) - 1 - stream->len, 0);

    if (r > 0) {
      stream->len += (size_t)r;
      stream->buff[stream->len] = '\0';
      return 1;
    }

    if (r == 0) {
      return 0;
    }
    if (errno == EINTR) {
      continue;
    }
    return -1;
  }

  return 0;
}

/*
 *
 * returns -1 if line to long 0 incomplite line
 * 1 if line is extracted
 *
 * */
int consumeLine(Stream *stream, char *out, size_t out_size) {
  char *nl = memchr(stream->buff, '\n', stream->len);
  if (nl == NULL) {
    return 0;
  }

  size_t end = (size_t)(nl - stream->buff);
  size_t n = end;
  if (n > 0 && stream->buff[n - 1] == '\r') {
    --n;
  }

	if (n + 1 > out_size) {
		return -1;
	}

	memcpy(out, &stream->buff, n);
	out[n] = '\0';

	memmove(stream->buff, stream->buff + end + 1, stream->len - end + 1);

	stream->len -= end + 1;
	stream->buff[stream->len] = '\0';

  return 1;
}
