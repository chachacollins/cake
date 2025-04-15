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

typedef enum {
    PARSE_SUCCESS,
    PARSE_ERROR,
} ParseResult;

ParseResult parseCakeFile(char* source, Rules* rules);
void freeRules(Rules* rules);

#endif // !PARSER_H
