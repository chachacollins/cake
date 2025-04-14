#include "parser.h"
#include "lexer.h"
#include <string.h>

typedef struct {
    Token current;
    Token previous;
} Parser;

Parser parser = {0};

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

static void addRule(Rules* rules, CakeRule rule)
{
    if(rules->capacity < rules->len + 1)
    {
        rules->capacity = GROW_CAPACITY(rules->capacity);
        rules->rules = ALLOCATE(CakeRule, rules->rules, rules->capacity * sizeof(rule));
    }
    rules->rules[rules->len++] = rule;
}

static void freeRule(Rules* rules)
{
    FREE(rules->rules);
    initRule(rules);
}

static CakeRule makeTarget(void)
{
    if(parser.current.kind != TOK_IDENT) 
        die("[Error] expected identifier found %*.s line:%d\n", parser.current.length, parser.current.start, parser.current.line);

    CakeRule target = {0};
    initSb(&target.commands);
    initSb(&target.deps);

    //Target
    target.target = takeStr(parser.current.start, parser.current.length);
    advance();

    //Dependancies
    if(parser.current.kind != TOK_COLON) 
    {
        die("[Error] expected colon found %*.s line:%d\n", parser.current.length, parser.current.start, parser.current.line);
    }
    advance();
    if(parser.current.kind != TOK_FAT_ARROW)
    {
        while (parser.current.kind != TOK_FAT_ARROW && parser.current.kind != TOK_EOF) {
            addSb(&target.deps, takeStr(parser.current.start, parser.current.length));
            advance();
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
            concat(&command, parser.current.start, parser.current.length);
            concat(&command, " ", 1); 
            advance();
        }
        if (command) 
        {
            size_t len = strlen(command);
            if (len > 0 && command[len - 1] == ' ')
                command[len - 1] = '\0';
            addSb(&target.commands, command);
        }
        if (parser.current.kind == TOK_COMMA)
            advance();
            
    }
    if(parser.current.kind != TOK_SEMICOLON)
        die("[Error] expected semicolon found %*.s line:%d\n", parser.current.length, parser.current.start, parser.current.line);
    advance();
    return target; 
}

Rules parseCakeFile(const char* source)
{
    Rules rules;
    initRule(&rules);
    initLexer(source);
    advance();
    while(parser.current.kind != TOK_EOF)
    {
        switch (parser.current.kind)
        {
            case TOK_IDENT: addRule(&rules, makeTarget()); break;
            default: 
                UNIMPLEMENTED;
        }
    }
    return rules;
}
void freeParser(Rules* rules)
{
    for(unsigned int i = 0; i < rules->len; ++i)
    {
        for(unsigned int j = 0; j < rules->rules[i].deps.len; ++j)
        {
            FREE(rules->rules[i].deps.strings[j]);
        }
        FREE(rules->rules[i].deps.strings);
        
        for(unsigned int j = 0; j < rules->rules[i].commands.len; ++j)
        {
            FREE(rules->rules[i].commands.strings[j]);
        }
        FREE(rules->rules[i].commands.strings);
        
        FREE(rules->rules[i].target);
    }
    freeRule(rules);
}
