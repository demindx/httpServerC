#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "stream.h"
#include "arena.h"

#define BUFF_SIZE 8192

struct Stream {
  int fd;
  char buff[BUFF_SIZE];
  size_t len;
};

void shiftBuff(Stream *stream, size_t from);

StreamStatus fill(Stream *stream);

Stream *createStream(MemoryArena *arena,int fd) {
  Stream *stream = (Stream *)arenaPush(arena, sizeof(Stream), false);

  if (stream == NULL) {
    fprintf(stderr, "ERROR(Stream): failed to alocate the stream.\n");
    return NULL;
  }

  stream->fd = fd;
  stream->len = 0;
  stream->buff[stream->len] = '\0';

  return stream;
}

void freeStream(Stream *stream) { free(stream); }

StreamStatus fill(Stream *stream) {
  if (stream->len >= sizeof(stream->buff)) {
    fprintf(stderr, "ERROR(Stream): stream is full.\n");
    return STREAM_TOO_LONG;
  }

  for (;;) {
    ssize_t recived = recv(stream->fd, stream->buff + stream->len,
                           sizeof(stream->buff) - 1 - stream->len, 0);

    if (recived > 0) {
      stream->len += (size_t)recived;
      stream->buff[stream->len] = '\0';

      return STREAM_OK;
    }

    if (recived == 0) {
      fprintf(stderr, "ERROR(Stream): EOF encountered\n");
      return STREAM_EOF;
    }

    if (errno == EINTR) {
      continue;
    }

    return STREAM_ERROR;
  }

  return STREAM_OK;
}

StreamStatus consumeLine(Stream *stream, char *out, size_t out_size) {
  if (out == NULL || out_size == 0) {
    fprintf(stderr, "ERROR(Stream): value error. \n");
    return STREAM_ERROR;
  }

  for (;;) {
    char *nl = memchr(stream->buff, '\n', stream->len);

    if (nl != NULL) {
      size_t n = (size_t)(nl - stream->buff) + 1;

      if (n >= out_size) {
        return STREAM_TOO_LONG;
      }

      memcpy(out, &stream->buff, n);

      out[n] = '\0';

      shiftBuff(stream, n);

      return STREAM_OK;
    }

    if (stream->len == sizeof(stream->buff)) {
      return STREAM_TOO_LONG;
    }

		if (stream->len == 0) {
			StreamStatus st = fill(stream);
			if (st != STREAM_OK) {
				return st;
			}
		}
  }
}

size_t readFromStream(Stream *stream, char *out, size_t n, StreamStatus *st) {
	*st = STREAM_OK;
  if (out == NULL) {
    fprintf(stderr, "Error(Stream): out buff is NULL \n");
    return STREAM_ERROR;
  }

  size_t readed = 0;

  while (n > readed) {
    if (stream->len == 0) {
      *st = fill(stream);

      if (*st != STREAM_OK) {
        fprintf(stderr, "ERROR(Stream): error in fill %d\n", *st);
        break;
      }
    }

    size_t to_copy = fmin(stream->len, n - readed);
    memcpy(out + readed, stream->buff, to_copy);

    readed += to_copy;

    shiftBuff(stream, to_copy);
  }

  out[readed] = '\0';
  return readed;
}

void shiftBuff(Stream *stream, size_t from) {
  if (from >= stream->len) {
    stream->len = 0;
    stream->buff[0] = '\0';
    return;
  }

  memmove(stream->buff, stream->buff + from, stream->len - from);

  stream->len -= from;
  stream->buff[stream->len] = '\0';
}

// char *readToTheEndOfStream(Stream *s) {
//   size_t cap = BUFF_SIZE * 2, len = 0;
//   char *res = malloc(cap);
//   if (!res) {
//     return NULL;
//   }
//
//   for (;;) {
//     if (cap - len <= BUFF_SIZE) {
//       char *tmp = realloc(res, cap * 2);
//       if (!tmp) {
//         free(res);
//         return NULL;
//       }
//       res = tmp;
//       cap *= 2;
//     }
//
// 		StreamStatus st;
//     size_t r = readFromStream(s, res + len, BUFF_SIZE, &st);
//
//     len += r;
//
// 		if (st == STREAM_EOF) {
// 			break;
// 		}
//
// 		if (st != STREAM_OK) {
// 			free(res);
// 			return NULL;
// 		}
//   }
//   res[len] = '\0';
//   return res;
// }
