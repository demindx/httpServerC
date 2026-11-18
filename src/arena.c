#include "arena.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARENA_HEADER_SIZE (sizeof(MemoryArena))
#define ARENA_ALIGN_POW(n, p) (((n) + (p) - 1) & ~((p) - 1))
#define ARENA_PAGESIZE (sizeof(void *))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

struct MemoryArena {
  size_t capacity;
  size_t pos;
};

MemoryArena *createArena(size_t size) {
  size_t total_size = size + ARENA_HEADER_SIZE;
  MemoryArena *arena = (MemoryArena *)malloc(total_size);

  if (!arena) {
    return NULL;
  }

  arena->capacity = size;
  arena->pos = ARENA_HEADER_SIZE;

  return arena;
}

void *arenaPush(MemoryArena *arena, size_t size, bool non_zero) {
  if (!arena) {
    fprintf(stderr, "ERROR(Arena): arena is NULL\n");
    return NULL;
  }

  size_t aligned_pos = ARENA_ALIGN_POW(arena->pos, ARENA_PAGESIZE);
  size_t new_pos = aligned_pos + size;

  if (new_pos > arena->capacity) {
    fprintf(stderr, "ERROR(Arena): size out of range\n");
    return NULL;
  }

  arena->pos = new_pos;
  void *out = (void *)arena + aligned_pos;

  if (!non_zero) {
    memset(out, 0, size);
  }

  return out;
}

void arenaPop(MemoryArena *arena, size_t size) {
  if (!arena) {
    fprintf(stderr, "ERROR(Arena): arena is NULL\n");
    return;
  }

  arena->pos -= MIN(size, arena->pos - ARENA_HEADER_SIZE);
}
void arenaClear(MemoryArena *arena) {
  if (!arena) {
    fprintf(stderr, "ERROR(Arena): arena is NULL\n");
    return;
  }

  arena->pos = ARENA_HEADER_SIZE;
}

void arenaFree(MemoryArena *arena) {
  if (!arena) {
    fprintf(stderr, "ERROR(Arena): arena is NULL\n");
    return;
  }

  free(arena);
}
