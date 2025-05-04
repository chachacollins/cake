#include "lexer.h"
#include <ctype.h>
#include <stdbool.h>

typedef struct {
    const char* start;
    const char* current;
    unsigned int line;
} Lexer;

Lexer lexer = {0};

void initLexer(const char* source)
{
    lexer.start = source;
    lexer.current = source;
    lexer.line = 1;
}

static char advance(void)
{
    lexer.current++;
    return lexer.current[-1];
}

static char peek(void)
{
    return *lexer.current;
}

static void skipWhiteSpace(void)
{
    for(;;)
    {
        char c = peek();
        switch(c)
        {
            case '\t':
            case '\r':
            case ' ':
                advance();
                break;
            case '\n':
                lexer.line++;
                advance();
                break;
            case '#':
                while(peek() != '\n' && *lexer.current != '\0')
                    advance();
                break;
            default: return;
        }
    }
}

static bool match(const char c)
{
    if(peek() != c) return false;
    advance();
    return true;
}

static Token makeToken(TokenType type)
{
   return  (Token) {
        .kind = type,
        .start = lexer.start,
        .length = (int)(lexer.current - lexer.start),
        .line = lexer.line,
   };
}

static bool isalphanum(const char c)
{
    return isalnum(c) || c == '_' || c == '.' || c == '-' || c == '/' || c == '@' || c == '~';
}

static Token readIdent(void)
{
    while(isalphanum(*lexer.current))
    {
        advance();
    }
    return makeToken(TOK_IDENT);
}

static Token makeVariable(void)
{
    while(isalphanum(*lexer.current))
    {
        advance();
    }
    return makeToken(TOK_VAR);
}


static Token string(void)
{
    while(peek() != '"' && *lexer.current != '\0') 
    {
        if(peek() == '\n') lexer.line++;
        advance();
    }
    if(*lexer.current == '\0') die("Unterminated string.");
    advance();
    return makeToken(TOK_STRING);
}


Token nextToken(void)
{
    skipWhiteSpace();
    lexer.start = lexer.current;
    const char c = advance();
    if(isalphanum(c)) return readIdent();
    switch(c)
    {
        case ';': return makeToken(TOK_SEMICOLON);
        case '=': return makeToken(match('>') ? TOK_FAT_ARROW : TOK_ILLEGAL);
        case ':': return makeToken(TOK_COLON);
        case ',': return makeToken(TOK_COMMA);
        case '<': return makeToken(match('-') ? TOK_ASSIGNMENT : TOK_ILLEGAL);
        case '$': return makeVariable();
        case '"': return string();
        case '\0': return makeToken(TOK_EOF);
        default : return makeToken(TOK_ILLEGAL);
    }
}

char* tokenStr(TokenType type)
{
    return tokens[type];
}

