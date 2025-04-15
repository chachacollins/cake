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
    Rules rules;
    ParseResult result = parseCakeFile(source, &rules);
    FREE(source);
    if(result != PARSE_SUCCESS) 
    {
        freeRules(&rules);
    }
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
    freeRules(&rules);
}
