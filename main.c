#include <stdio.h>
#include "utils.h"
#include "lexer.h"

int main(void)
{
    //NOTE: DO NOT FORGET TO FREE THIS
    char* source = readFile("CAKEFILE");
    initLexer(source);
    Token tok = nextToken();
    do {
        printf("Token kind: %d line: %d chars:%.*s\n\n", tok.kind, tok.line, tok.length, tok.start);
        tok = nextToken();
    }while (tok.kind != TOK_EOF);
    FREE(source);
}
