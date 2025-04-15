#include <stdio.h>
#include <unistd.h>
#include "utils.h"
#include "parser.h"

static void usage(void)
{
    fprintf(stderr,"[Usage]");
}


int main(int argc, char* argv[])
{
    int opt;
    #ifdef DEBUG_H
        mem_summary();
    #endif /* ifdef DEBUG_H */
    char* filename = NULL;
    while((opt = getopt(argc, argv, "hf:")) != -1)
    {
        switch(opt)
        {
            case 'f': filename = optarg; break;
            case 'h': usage(); return 0;
            case '?': 
                fprintf(stderr, "[Error]: Unknown command\n");
                return 1; 
        }
    }
    
    char* source = readFile((filename ? filename : "CAKEFILE"));
    Rules rules;
    ParseResult result = parseCakeFile(source, &rules);
    FREE(source);
    if(result != PARSE_SUCCESS) 
    {
        freeRules(&rules);
        return 1; 
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
    return 0;
}
