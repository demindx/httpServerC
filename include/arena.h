#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define KiB(n) ((uint64_t)(n) << 10)
#define MiB(n) ((uint64_t)(n) << 20)
#define GiB(n) ((uint64_t)(n) << 30)

typedef struct MemoryArena MemoryArena;

MemoryArena *createArena(size_t size);

void *arenaPush(MemoryArena *arena, size_t size, bool non_zero);
void arenaPop(MemoryArena *arena, size_t size);
void arenaClear(MemoryArena *arena);
void arenaFree(MemoryArena *arena);
char *arenaStrdup(MemoryArena *arena, char *src);
