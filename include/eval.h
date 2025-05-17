#ifndef EVAL_H
#define EVAL_H

#include "parser.h"

typedef enum {
    EVAL_SUCCESS,
    EVAL_ERROR
} EvalResult;

FaireRule* findRule(Rules* rule, char* target);
EvalResult buildRule(Rules* rule,FaireRule* target, bool dry_run);
#endif // !EVAL_H
