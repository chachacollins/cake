#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

#define NORETURN __attribute__((noreturn))
#define UNIMPLEMENTED (die("This part is not implemented line:%d file:%s\n", __LINE__, __FILE__))
#define GROW_CAPACITY(capacity) ((capacity < 8) ? 8 : (capacity * 2))

#ifdef  __OPTIMIZE__

#define ALLOCATE(type, ptr, size) ((type*)reallocate(ptr, size))
#define FREE(ptr) (reallocate(ptr, 0))

#else 
#include "debug.h"

#define ALLOCATE(type, ptr, size) ((type*)dbg_alloc(ptr, size, __FILE__, __LINE__))
#define FREE(ptr) (dbg_free(ptr, __FILE__, __LINE__))

#endif //  __OPTIMIZE__

typedef struct {
    unsigned int len;
    unsigned int capacity; 
    char** strings;
} Sb;


NORETURN void die(const char* fmt, ...);
void* reallocate(void* ptr, size_t size);
char* readFile(const char* path);
void initSb(Sb* sb);
void addSb(Sb* sb, char* string);
char* takeStr(const char* str, unsigned int len);
void concat(char** dest, const char* src, int len);

#endif // !UTILS_H
