#ifndef QB_PARSER_H
#define QB_PARSER_H

#include "qb_lexer.h"
#include "qb_da.h"

struct Parser {
    struct Lexer lexer;
    struct Token current_token;
};

void QB_parser_init(struct Parser *parser, const char *src);
void QB_parser_advance(struct Parser *parser);
int  QB_parser_check(struct Parser *parser, enum TokenType type);
void QB_parser_eat(struct Parser *parser, enum TokenType type);

// Parser functions
Node* QB_parse_block(struct Parser *parser);
Node* QB_parse_funtion_dec(struct Parser *parser);

#endif //QB_PARSER_H
