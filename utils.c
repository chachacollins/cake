#include <stdarg.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>

#include "utils.h"

NORETURN void die(const char* fmt, ...)
{
    va_list  ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    exit(1);
}

void* reallocate(void* ptr, size_t size)
{
    if(size == 0) 
    {
         free(ptr);
        return NULL;
    }  
    void* result = realloc(ptr, size);
    if(!result) die("[Error]:could not allocate memory\n");
    return result;
}

char* readFile(const char* path)
{
    assert(strlen(path) > 0);
    errno = 0;
    FILE *file = fopen(path, "rb");
    if(!file)
    {
        die("[Error]:%s", strerror(errno));
    }
    fseek(file,0l,SEEK_END);
    size_t file_size =  ftell(file);
    rewind(file);
    char *buffer = ALLOCATE(char, NULL, file_size + 1);
    size_t bytes_read = fread(buffer, sizeof(char), file_size, file);
    if(bytes_read != file_size)
    {
        die("Failed to read CAKEFILE to memory; bytes read:%d file size:%d\n", bytes_read, file_size);
    }
    buffer[file_size] = '\0';
    return buffer;
}
