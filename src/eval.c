#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <time.h>
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

        printf("[Running]: %s\n", commands->strings[i]);

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
static bool getModTime(const char* filename, time_t *modtime)
{
    struct stat file_stat;
    if(stat(filename, &file_stat) == 0)
    {
        *modtime = file_stat.st_mtime;
        return true;
    }
    return false;
}

static bool shouldRebuild(CakeRule* target)
{
    if(target->phony) return true;
    time_t target_time;
    bool target_exits = getModTime(target->target, &target_time);
    if(!target_exits) return true;
    for(unsigned int i = 0; i < target->deps.len; ++i)
    {
        time_t deps_time;
        if(getModTime(target->deps.strings[i], &deps_time) && difftime(deps_time,target_time) > 0)
        {
            return true;
        }
    }
    return false;
}

EvalResult buildRule(Rules* rules, CakeRule* target)
{
    bool dependency_rebuilt = false;
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
                if (rule->was_rebuilt) {
                    dependency_rebuilt = true;
                }
            }
        }
    }

    bool needs_rebuild = target->phony || dependency_rebuilt || shouldRebuild(target);
    
    if(needs_rebuild)
    {
        target->was_rebuilt = true;
        return execCommand(&target->commands);
    } else {
        printf("Target '%s' is up to date\n", target->target);
        target->was_rebuilt = false;
        return EVAL_SUCCESS;
    }
}
