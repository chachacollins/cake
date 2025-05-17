#ifndef PARSER_H
#define PARSER_H

#include "utils.h"
#include <stdbool.h>

typedef struct {
    char* target;
    bool phony;
    bool was_rebuilt;
    Sb deps;
    Sb  commands;
}FaireRule;

typedef struct {
    unsigned int len;
    unsigned int capacity;
    FaireRule* rules;
}Rules;

typedef enum {
    PARSE_SUCCESS,
    PARSE_ERROR,
} ParseResult;

ParseResult parseFaireFile(char* source, Rules* rules);
void freeRules(Rules* rules);

#endif // !PARSER_H
