#ifndef DEBUG_H
#define DEBUG_H
#include <stdio.h>

void* dbg_alloc(void* ptr, size_t size, char* file, int line);
void dbg_free(void* ptr, char* file, int line);
void mem_summary(void);
#endif // !DEBUG_H
