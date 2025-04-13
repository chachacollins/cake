#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

#define NORETURN __attribute__((noreturn))
#define UNIMPLEMENTED (die("This part is not implemented line:%d file:%s\n", __LINE__, __FILE__))
#define ALLOCATE(type, ptr, size) ((type*)reallocate(ptr, size))
#define FREE(ptr) (reallocate(ptr, 0))


NORETURN void die(const char* fmt, ...);
void* reallocate(void* ptr, size_t size);
char* readFile(const char* path);

#endif // !UTILS_H
