#include <stdlib.h>
#include "debug.h"

#define MAX_ALLOC 1024

typedef struct {
    void* ptr;
    size_t size;
    char* file;
    int line;
} Allocation;


Allocation allocations[MAX_ALLOC] = {0};

static int alloc_count = 0;

void* dbg_alloc(void* ptr, size_t size, char* file, int line)
{
    if(size == 0)
    {
        free(ptr);
        return NULL;
    }
    void* alloc = realloc(ptr, size);
    if(!alloc) return NULL;
    if(alloc_count > MAX_ALLOC)
    {
        fprintf(stderr,"Too many allocations\n");
    }
    allocations[alloc_count++] = (Allocation){alloc, size,file,line};
    return alloc;
}

void dbg_free(void* ptr, char* file, int line)
{
    int pos = -1;
    for(int i = 0; i < alloc_count; ++i)
    {
        if (allocations[i].ptr == ptr)
        {
            free(allocations[i].ptr);
            pos = i;
            break;
        }
    }
    if(pos != -1) 
    {
        for(int i = pos; i < alloc_count - 1; ++i)
        {
            allocations[i] = allocations[i + 1];
        }
        alloc_count--;
        return;
    }
    printf("Free operation on unknown ptr:(%p) line: %d file: %s\n", ptr, line, file);
}

static void mem_dbg(void)
{
    if(alloc_count > 0)
    {
        for(int i = 0; i < alloc_count; ++i)
        {
            Allocation a = allocations[i];
            printf("[Warning]: memory leak size %zu bytes detected at address: %p line:%d file: %s\n",
                   a.size, a.ptr, a.line, a.file);
        }
    }
}

void mem_summary(void)
{
    atexit(mem_dbg);
}


