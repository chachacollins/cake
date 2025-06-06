#include <search.h>
#include "parser.h"
#include "lexer.h"
#include <string.h>

typedef struct {
    Token current;
    Token previous;
    ENTRY *globals;
    Sb alloced;
    int g_count;
} Parser;

Parser parser = {0};
static void freeParser(void);

static void advance(void)
{
    parser.previous = parser.current;
    parser.current = nextToken();
}

static void initRule(Rules* rules)
{
    rules->len = 0;
    rules->capacity = 0;
    rules->rules = NULL;
}

static void addRule(Rules* rules, FaireRule rule)
{
    if(rules->capacity < rules->len + 1)
    {
        rules->capacity = GROW_CAPACITY(rules->capacity);
        rules->rules = ALLOCATE(FaireRule, rules->rules, rules->capacity * sizeof(rule));
    }
    rules->rules[rules->len++] = rule;
}

static void freeRule(Rules* rules)
{
    FREE(rules->rules);
    initRule(rules);
}

static ParseResult makeTarget(FaireRule *target)
{
    if(parser.current.kind != TOK_IDENT) 
    {
        fprintf(stderr,"[Error] expected identifier found %.*s line:%d\n", 
                parser.current.length, parser.current.start, parser.current.line);
        return PARSE_ERROR;
    }

    initSb(&target->commands);
    initSb(&target->deps);

    //Target
    char* targetstr = takeStr(parser.current.start, parser.current.length);
    if(*targetstr == '@') 
    {
        target->phony = true;
        target->target = targetstr + 1;
    } else {
        target->phony = false;
        target->target = targetstr;
    }
    advance();

    //Dependancies
    if(parser.current.kind != TOK_COLON) 
    {
        fprintf(stderr,"[Error] expected colon found %.*s line:%d\n", 
                parser.current.length, parser.current.start, parser.current.line);
        return PARSE_ERROR;
    }
    advance();
    if(parser.current.kind != TOK_FAT_ARROW)
    {
        while (parser.current.kind != TOK_FAT_ARROW && parser.current.kind != TOK_EOF) {
            if(parser.current.kind == TOK_VAR)
            {
                char* key = takeStr(parser.current.start + 1, parser.current.length - 1);
                ENTRY e;
                e.key = key;
                parser.globals = hsearch(e, FIND);
                if(parser.globals == NULL) 
                {
                    fprintf(stderr,"Variable %.*s not found line:%d\n",
                        parser.current.length - 1, parser.current.start + 1, parser.current.line);
                    return PARSE_ERROR;
                }
                addSb(&target->deps, takeStr((char*)parser.globals->data, strlen((char*)parser.globals->data)));
                FREE(key);
                advance();
            } else {
                addSb(&target->deps, takeStr(parser.current.start, parser.current.length));
                advance();
            }
        }
    }
    advance();

    //Commands
    while (parser.current.kind != TOK_SEMICOLON && parser.current.kind != TOK_EOF) 
    {
        char* command = NULL;
        while (parser.current.kind != TOK_COMMA &&
            parser.current.kind != TOK_SEMICOLON &&
            parser.current.kind != TOK_EOF)
        {
            if(parser.current.kind == TOK_VAR)
            {
                char* key = takeStr(parser.current.start + 1, parser.current.length - 1);
                ENTRY e;
                e.key = key;
                parser.globals = hsearch(e, FIND);
                if(parser.globals == NULL) 
                {
                    fprintf(stderr,"Variable %.*s not found line:%d\n",
                        parser.current.length - 1, parser.current.start + 1, parser.current.line);
                    return PARSE_ERROR;
                }
                concat(&command, (char*)parser.globals->data, strlen(parser.globals->data));
                concat(&command, " ", 1); 
                FREE(key);
                advance();
            } else {
                concat(&command, parser.current.start, parser.current.length);
                concat(&command, " ", 1); 
                advance();
            }
        }
        if (command) 
        {
            size_t len = strlen(command);
            if (len > 0 && command[len - 1] == ' ')
                command[len - 1] = '\0';
            addSb(&target->commands, command);
        }
        if (parser.current.kind == TOK_COMMA)
            advance();
            
    }
    if(parser.current.kind != TOK_SEMICOLON)
    {
        fprintf(stderr,"[Error] expected semicolon found %.*s line:%d\n", 
                parser.current.length, parser.current.start, parser.current.line);
        return PARSE_ERROR;
    }
    advance();
    target->was_rebuilt = false;
    return PARSE_SUCCESS; 
}

static ParseResult makeVariable(void)
{
   if(parser.g_count > 100)
   {
        fprintf(stderr,"[Error]: Too many variables declared");
        return PARSE_ERROR;
   }
   ENTRY e; 
   char* key = takeStr(parser.current.start + 1, parser.current.length - 1);
   e.key = key;
   addSb(&parser.alloced, key);
   advance();
   if(parser.current.kind != TOK_ASSIGNMENT)
   {
        fprintf(stderr,"[Error] expected <- found %.*s line:%d\n",
            parser.current.length, parser.current.start, parser.current.line);
        return PARSE_ERROR;
   }

   advance();
   if(parser.current.kind != TOK_STRING)
   { 
        fprintf(stderr,"[Error] expected string found %.*s line:%d\n", 
                parser.current.length, parser.current.start, parser.current.line);
        return PARSE_ERROR;
    }

   char *data = takeStr(parser.current.start + 1, parser.current.length - 2);
   addSb(&parser.alloced, data);
   e.data = (void *)data;
   parser.globals = hsearch(e, ENTER);
   if(parser.globals == NULL)
   {
        fprintf(stderr,"'This will never fail' said a naive dev\n");
        return PARSE_ERROR;
   }
   advance();
   parser.g_count++;
   return PARSE_SUCCESS;
}


ParseResult parseFaireFile(char* source, Rules* rules)
{
    hcreate(100);
    initRule(rules);
    initLexer(source);
    advance();
    FaireRule target;
    while(parser.current.kind != TOK_EOF)
    {
        switch (parser.current.kind)
        {
            case TOK_IDENT: 
                if(makeTarget(&target) != PARSE_SUCCESS)
                {
                    freeParser();
                    return PARSE_ERROR;
                }
                addRule(rules, target);
                break;
            case TOK_VAR:
                if(makeVariable() != PARSE_SUCCESS)
                {
                    freeParser();
                    return PARSE_ERROR;
                }
                break;
            default: 
               fprintf(stderr, "This is caused an error in the parser %*.s line:%d\n",
                       parser.current.length, parser.current.start, parser.current.line);

               freeParser();
               return PARSE_ERROR;
        }
    }
    freeParser();
    return PARSE_SUCCESS;
}
static void freeParser(void)
{
    hdestroy();
    for(unsigned int i = 0; i < parser.alloced.len; ++i)
    {
        FREE(parser.alloced.strings[i]);
    }
    FREE(parser.alloced.strings);
}

void freeRules(Rules* rules)
{
    for(unsigned int i = 0; i < rules->len; ++i)
    {
        for(unsigned int j = 0; j < rules->rules[i].deps.len; j++)
        {
            if(rules->rules[i].deps.strings[j])
                FREE(rules->rules[i].deps.strings[j]);
        }
        if(rules->rules[i].deps.strings)
            FREE(rules->rules[i].deps.strings);

        for(unsigned int j = 0; j < rules->rules[i].commands.len; ++j)
        {
            FREE(rules->rules[i].commands.strings[j]);
        }
        FREE(rules->rules[i].commands.strings);

        if(rules->rules[i].phony == true)
        {
            FREE(rules->rules[i].target - 1);
        } else {
            FREE(rules->rules[i].target);
        }
    }
    freeRule(rules);
}
