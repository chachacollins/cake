#ifndef LEXER_H
#define LEXER_H

#include "utils.h"

#define TOKENS         \
    X(TOK_IDENT)       \
    X(TOK_FAT_ARROW)   \
    X(TOK_SEMICOLON)   \
    X(TOK_COLON)       \
    X(TOK_COMMA)       \
    X(TOK_ASSIGNMENT)  \
    X(TOK_STRING)      \
    X(TOK_VAR)         \
    X(TOK_EOF)         \
    X(TOK_ILLEGAL)

#define X(name) name,
typedef enum {
    TOKENS
} TokenType;
#undef X

#define X(name) #name,
static char* tokens[] = {TOKENS};
#undef X

typedef struct {
    TokenType    kind;
    unsigned int length;
    unsigned int line;
    const char*  start;
} Token;


void initLexer(const char* source);
Token nextToken(void);
char* tokenStr(TokenType type);


#endif // !LEXER_H
