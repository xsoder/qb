#ifndef QB_PARSER_H
#define QB_PARSER_H

#include "qb_lexer.h"
#include "qb_da.h"

struct VarEntry {
    char *name;
    struct VarEntry *next;
};

struct Parser {
    struct Lexer lexer;
    struct Token current_token;
    struct VarEntry *vars;
};

void QB_parser_init(struct Parser *parser, const char *src);
void QB_parser_advance(struct Parser *parser);
int  QB_parser_check(struct Parser *parser, enum TokenType type);
void QB_parser_eat(struct Parser *parser, enum TokenType type);

// Varaible checkers
void QB_add_var(struct Parser *parser, const char* name);
int  QB_var_exists(struct Parser *parser, const char* name);

// Parser functions
Node *QB_parse_block(struct Parser *parser);
Node *QB_parse_funtion_dec(struct Parser *parser);
Node *QB_parse_variable_dec(struct Parser *parser);
Node *QB_parser_expression(struct Parser *parser);
Node *QB_parser_assigment(struct Parser *parser);
Node *QB_parser_addition(struct Parser *parser);

#endif //QB_PARSER_H
