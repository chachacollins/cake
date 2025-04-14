#ifndef PARSER_H
#define PARSER_H

#include "utils.h"

typedef struct {
    char* target;
    Sb deps;
    Sb  commands;
}CakeRule;

typedef struct {
    unsigned int len;
    unsigned int capacity;
    CakeRule* rules;
}Rules;

Rules parseCakeFile(const char* source);
void freeParser(Rules* rules);

#endif // !PARSER_H
