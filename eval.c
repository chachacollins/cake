#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include "utils.h"
#include "eval.h"

CakeRule* findRule(Rules* rules, char* target)
{
    for(unsigned int i = 0; i < rules->len; ++i)
    {
        if(strcmp(target, rules->rules[i].target) == 0)
        {
            return &rules->rules[i];
        }
    }
    return NULL;
}

static EvalResult execCommand(Sb* commands)
{
    for(unsigned int i = 0; i < commands->len; ++i)
    {
        char* cmd_copy = takeStr(commands->strings[i], strlen(commands->strings[i]));
        if (!cmd_copy) {
            perror("[Error] Memory allocation failed");
            return EVAL_ERROR;
        }

        int token_count = 0;
        char* temp = takeStr(cmd_copy, strlen(cmd_copy));
        if (!temp) {
            FREE(cmd_copy);
            perror("[Error] Memory allocation failed");
            return EVAL_ERROR;
        }

        char* token = strtok(temp, " ");
        while(token != NULL) {
            token_count++;
            token = strtok(NULL, " ");
        }
        FREE(temp);

        char** args = ALLOCATE(char*, NULL, sizeof(char*) * (token_count + 1));
        if (!args) {
            FREE(cmd_copy);
            perror("[Error] Memory allocation failed");
            return EVAL_ERROR;
        }

        token = strtok(cmd_copy, " ");
        int arg_index = 0;
        while(token != NULL && arg_index < token_count) {
            args[arg_index++] = token;
            token = strtok(NULL, " ");
        }
        args[arg_index] = NULL;

        if (arg_index == 0) {
            FREE(cmd_copy);
            FREE(args);
            continue; 
        }

        printf("Running command: %s\n", commands->strings[i]);

        errno = 0;
        pid_t pid = fork();
        int status;
        switch (pid)
        {
            case -1: 
                perror("[Error]");
                FREE(cmd_copy);
                FREE(args);
                return EVAL_ERROR;
            case 0:
                errno = 0;
                execvp(args[0], args);
                perror("[Error]");
                return EVAL_ERROR;
                break;
            default:
                if (waitpid(pid, &status, 0) == -1) {
                    perror("[Error] waitpid failed");
                    FREE(cmd_copy);
                    FREE(args);
                    return EVAL_ERROR;
                }

                if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
                    fprintf(stderr, "[Error] Command failed with exit code %d\n", WEXITSTATUS(status));
                    FREE(cmd_copy);
                    FREE(args);
                    return EVAL_ERROR;
                }
                break;
        }

        FREE(cmd_copy);
        FREE(args);
    }
    return EVAL_SUCCESS;
}

EvalResult buildRule(Rules* rules, CakeRule* target)
{
    if(target->deps.len != 0)
    {
        for (unsigned int i = 0; i < target->deps.len; i++) {
            CakeRule* rule = findRule(rules, target->deps.strings[i]);
            if(rule != NULL)
            { 
                EvalResult result = buildRule(rules, rule);
                if (result != EVAL_SUCCESS) {
                    return result;
                }
            }
        }
    }
    return execCommand(&target->commands);
}
