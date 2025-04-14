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
    vfprintf(stderr,fmt, ap);
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
        fclose(file);
        die("Failed to read CAKEFILE to memory; bytes read:%d file size:%d\n", bytes_read, file_size);
    }
    buffer[file_size] = '\0';
    fclose(file);
    return buffer;
}

void initSb(Sb* sb)
{
    sb->len = 0;
    sb->capacity = 0;
    sb->strings = NULL;
}

void addSb(Sb* sb, char* string)
{
    if(sb->capacity < sb->len + 1)
    {
        sb->capacity = GROW_CAPACITY(sb->capacity);
        sb->strings = ALLOCATE(char*, sb->strings, sb->capacity * sizeof(char*));
    }
    sb->strings[sb->len++] = string;
}


char* takeStr(const char* str, unsigned int len)
{
    assert(str != NULL);
    char* buffer = ALLOCATE(char, NULL, len + 1);
    memcpy(buffer, str, len);
    buffer[len] = '\0';
    return buffer;
}

void concat(char** dest, const char* src, int len)
{
    size_t old_len = *dest ? strlen(*dest) : 0;
    size_t new_len = old_len + len;
    char* buffer = ALLOCATE(char, NULL, new_len + 1);
    if (*dest) 
    {
        memcpy(buffer, *dest, old_len);
        FREE(*dest); 
    }
    memcpy(buffer + old_len, src, len);
    buffer[new_len] = '\0';
    *dest = buffer;
}
