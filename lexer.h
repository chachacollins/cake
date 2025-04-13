#ifndef LEXER_H
#define LEXER_H

typedef enum {
    TOK_IDENT,
    TOK_FAT_ARROW,
    TOK_SEMICOLON,
    TOK_COLON,
    TOK_EOF,
    TOK_ILLEGAL,
} TokenType;

typedef struct {
    TokenType    kind;
    unsigned int length;
    unsigned int line;
    const char*  start;
} Token;


void initLexer(const char* source);
Token nextToken(void);

#endif // !LEXER_H
