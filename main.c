#include <stdio.h>
#include "utils.h"
#include "parser.h"


int main(void)
{
    #ifdef DEBUG_H
        mem_summary();
    #endif /* ifdef DEBUG_H */
    char* source = readFile("CAKEFILE");
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
