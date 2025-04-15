#include <stdio.h>
#include <unistd.h>
#include "utils.h"
#include "parser.h"
#include "eval.h"

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
    if(argc < 2)
    {
        usage();
    }
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
    CakeRule* target = findRule(&rules, argv[1]);
    if(!target) 
    {
        fprintf(stderr, "[Error]: Target %s not found\n", argv[1]);
        freeRules(&rules);
    }
    EvalResult ret = buildRule(&rules, target);
    if(ret != EVAL_SUCCESS)
    {
        freeRules(&rules);
    }
    freeRules(&rules);
    return 0;
}
