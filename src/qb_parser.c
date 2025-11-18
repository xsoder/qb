#include "qb_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void QB_parser_init(struct Parser *parser, const char *src)
{
    QB_init_lexer(&parser->lexer, src);
    parser->current_token = QB_match_token(&parser->lexer, &parser->current_token);
}

void QB_parser_advance(struct Parser *parser)
{
    parser->current_token = QB_match_token(&parser->lexer, &parser->current_token);
}

int QB_parser_check(struct Parser *parser, enum TokenType type)
{
    return parser->current_token.type == type;
}

void QB_parser_eat(struct Parser *parser, enum TokenType type) {
    if (QB_parser_check(parser, type)) {
        QB_parser_advance(parser);
    } else {
        fprintf(stderr, "[ERROR]: Unexpected token, expected type %d but got type %d\n",
                type, parser->current_token.type);
        exit(EXIT_FAILURE);
    }
}

Node* QB_parse_block(struct Parser *parser) {
    Node* block_node = malloc(sizeof(Node));
    block_node->type = NODE_BLOCK;
    block_node->block_node.statements = NULL;
    block_node->block_node.count = 0;

    while (!QB_parser_check(parser, TOKEN_RBRACE) && !QB_parser_check(parser, TOKEN_EOF)) {
        QB_parser_advance(parser);
    }

    return block_node;
}

Node *QB_parse_funtion_dec(struct Parser *parser)
{
    if (!QB_parser_check(parser, TOKEN_IDENTIFIER)) {
        fprintf(stderr, "[ERROR]: Expected function name\n");
        exit(EXIT_FAILURE);
    }

    char *function_name = strndup(parser->current_token.start, parser->current_token.len);
    QB_parser_eat(parser, TOKEN_IDENTIFIER);

    QB_parser_eat(parser, TOKEN_COLON);
    QB_parser_eat(parser, TOKEN_COLON);

    if (QB_parser_check(parser, TOKEN_IDENTIFIER)) {
        QB_parser_eat(parser, TOKEN_IDENTIFIER);
    }
    QB_parser_eat(parser, TOKEN_LBRACE);

    Node *body = QB_parse_block(parser);

    QB_parser_eat(parser, TOKEN_RBRACE);

    Node *function_node = malloc(sizeof(Node));
    function_node->type = QB_FUNCTION;
    function_node->function_node.name = function_name;
    function_node->function_node.body = body;

    return function_node;
}
