#include <stdio.h>
#include <unistd.h>
#include "utils.h"
#include "parser.h"
#include "eval.h"

static void usage(void)
{
    fprintf(stderr, "Usage: cake [OPTIONS] TARGET\n\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -f FILENAME  Specify build file (default: CAKEFILE)\n");
    fprintf(stderr, "  -h           Display this help message and exit\n\n");
    fprintf(stderr, "Arguments:\n");
    fprintf(stderr, "  TARGET       Name of the target to build\n");
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
        return 1;
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
    Rules rules = {0};
    ParseResult result = parseCakeFile(source, &rules);
    FREE(source);
    if(result != PARSE_SUCCESS) 
    {
        goto error;
        return 1; 
    }
    CakeRule* target = findRule(&rules, argv[1]);
    if(!target) 
    {
        fprintf(stderr, "[Error]: Target %s not found\n", argv[1]);
        goto error;
        return 1;
    }
    EvalResult ret = buildRule(&rules, target);
    if(ret != EVAL_SUCCESS)
    {
        goto error;
    }
    freeRules(&rules);
    return 0;
error:
    if(rules.len > 0) freeRules(&rules);
    return 1;
}
