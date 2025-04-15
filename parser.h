#ifndef PARSER_H
#define PARSER_H

#include "utils.h"
#include <stdbool.h>

typedef struct {
    char* target;
    bool phony;
    Sb deps;
    Sb  commands;
}CakeRule;

typedef struct {
    unsigned int len;
    unsigned int capacity;
    CakeRule* rules;
}Rules;

Rules parseCakeFile(char* source);
void freeParser(Rules* rules);

#endif // !PARSER_H
