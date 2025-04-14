#include <stdio.h>
#include "utils.h"
#include "parser.h"
#include "lexer.h"


int main(void)
{
    #ifdef DEBUG_H
        mem_summary();
    #endif /* ifdef DEBUG_H */
    char* source = readFile("CAKEFILE");
    // initLexer(source);
    // Token tok = nextToken();
    // do {
    //     printf("Token kind: %s line: %d chars:'%.*s'\n", tokenStr(tok.kind), tok.line, tok.length, tok.start);
    //     tok = nextToken();
    // }while (tok.kind != TOK_EOF);
    Rules rules = parseCakeFile(source);
    for(unsigned int i = 0; i < rules.len; i++)
    {
        CakeRule rule = rules.rules[i];
        printf("---------------\n");
        printf("Target: %s \n", rule.target);
        printf("Deps:  ");
        for(unsigned int i = 0; i < rule.deps.len; i++)
        {
            printf("%s ", rule.deps.strings[i]);
        }
        printf("\n");
        printf("commands len %d:  ", rule.commands.len);
        for(unsigned int i = 0; i < rule.commands.len; i++)
        {
            printf("'%s'", rule.commands.strings[i]);
        }
        printf("\n");
        printf("---------------\n");
    }
    freeParser(&rules);
    FREE(source);
}
