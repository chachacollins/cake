#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "utils.h"
#include "parser.h"
#include "eval.h"

static void usage(void)
{
    fprintf(stderr, "Usage: faire [OPTIONS] TARGET\n\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -d           Dry run the specified Rule\n");
    fprintf(stderr, "  -h           Display this help message and exit\n\n");
    fprintf(stderr, "Arguments:\n");
    fprintf(stderr, "  TARGET       Name of the target to build\n");
}

static char* sanitizeRule(char* rule)
{
    if(rule[0] == '@')
    {
        rule = strtok(rule, "@");
    }
    rule = strtok(rule, " ");
    return rule;
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
    char* rule = NULL;
    bool dry_run = false;
    while((opt = getopt(argc, argv, "hd")) != -1)
    {
        switch(opt)
        {
            case 'h':
                usage(); 
                return 0;
            case 'd': 
                if(optind > argc)
                {
                    fprintf(stderr, "[Error]: expected rule name after -d\n");
                    goto error;
                }
                rule = argv[optind]; 
                dry_run = true;
                break;
            case '?': 
                fprintf(stderr, "[Error]: Unknown command\n");
                goto error;
        }
    }
    
    char* source = readFile((filename ? filename : "FAIRE"));
    Rules rules = {0};
    ParseResult result = parseFaireFile(source, &rules);
    FREE(source);
    if(result != PARSE_SUCCESS) 
    {
        goto error;
    }
    if(!dry_run) rule = argv[1];
    rule = sanitizeRule(rule);
    FaireRule* target = findRule(&rules, rule);
    if(!target) 
    {
        fprintf(stderr, "[Error]: Target %s not found\n", rule);
        goto error;
    }
    EvalResult ret = buildRule(&rules, target, dry_run);
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
