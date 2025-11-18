#include "qb_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void QB_parser_init(struct Parser *parser, const char *src)
{
    QB_init_lexer(&parser->lexer, src);
    parser->current_token = QB_match_token(&parser->lexer, &parser->current_token);
    parser->vars = NULL;
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

Node* QB_parse_primary(struct Parser *parser) {
    if (QB_parser_check(parser, TOKEN_NUMBER)) {
        char *num_str = strndup(parser->current_token.start, parser->current_token.len);
        int value = atoi(num_str);
        free(num_str);

        QB_parser_eat(parser, TOKEN_NUMBER);

        return create_number_node(value);
    }

    if (QB_parser_check(parser, TOKEN_IDENTIFIER)) {
        char *name = strndup(parser->current_token.start, parser->current_token.len);
        QB_parser_eat(parser, TOKEN_IDENTIFIER);

        Node *node = malloc(sizeof(Node));
        node->type = NODE_IDENTIFIER;
        node->identifer_node.name = name;
        return node;
    }

    fprintf(stderr, "[ERROR]: Expected expression\n");
    exit(EXIT_FAILURE);
}

Node* QB_parse_multiplication(struct Parser *parser) {
    Node *left = QB_parse_primary(parser);

    while (QB_parser_check(parser, TOKEN_ASTERIK)) {
        struct Token op = parser->current_token;
        QB_parser_advance(parser);
        Node *right = QB_parse_primary(parser);
        left = create_binary_op_node(left, &op, right);
    }

    return left;
}

Node* QB_parse_addition(struct Parser *parser) {
    Node *left = QB_parse_multiplication(parser);

    while (QB_parser_check(parser, TOKEN_PLUS)) {
        struct Token op = parser->current_token;
        QB_parser_advance(parser);
        Node *right = QB_parse_multiplication(parser);
        left = create_binary_op_node(left, &op, right);
    }

    return left;
}

Node* QB_parse_subtraction(struct Parser *parser) {
    Node *left = QB_parse_addition(parser);

    while (QB_parser_check(parser, TOKEN_SUBTRACT)) {
        struct Token op = parser->current_token;
        QB_parser_advance(parser);
        Node *right = QB_parse_addition(parser);
        left = create_binary_op_node(left, &op, right);
    }

    return left;
}

Node* QB_parser_expression(struct Parser *parser) {
    return QB_parse_subtraction(parser);
}

Node* QB_parse_block(struct Parser *parser) {
    Node* block_node = malloc(sizeof(Node));
    block_node->type = NODE_BLOCK;
    block_node->block_node.statements = NULL;
    block_node->block_node.count = 0;
    int capacity = 0;

    while (!QB_parser_check(parser, TOKEN_RBRACE) && !QB_parser_check(parser, TOKEN_EOF)) {
        Node *statement = NULL;

        if (QB_parser_check(parser, TOKEN_IDENTIFIER)) {
            struct Lexer saved_lexer = parser->lexer;
            struct Token first_token = parser->current_token;

            QB_parser_advance(parser);

            if (QB_parser_check(parser, TOKEN_IDENTIFIER)) {
                parser->lexer = saved_lexer;
                parser->current_token = first_token;
                statement = QB_parse_variable_dec(parser);
            } else if (QB_parser_check(parser, TOKEN_EQ)) {
                parser->lexer = saved_lexer;
                parser->current_token = first_token;
                statement = QB_parser_assigment(parser);
            } else {
                fprintf(stderr, "[ERROR]: Unexpected token after identifier\n");
                exit(EXIT_FAILURE);
            }
        } else {
            fprintf(stderr, "[ERROR]: Skipping unknown token in block\n");
            exit(EXIT_FAILURE);
        }

        if (statement) {
            if (block_node->block_node.count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                block_node->block_node.statements = realloc(
                    block_node->block_node.statements,
                    capacity * sizeof(Node*)
                );
            }
            block_node->block_node.statements[block_node->block_node.count++] = statement;
        }

        if (QB_parser_check(parser, TOKEN_SEMICOLON)) {
            QB_parser_eat(parser, TOKEN_SEMICOLON);
        } else if (QB_parser_check(parser, TOKEN_RBRACE)) {
            break;
        }
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

    // TODO: accept argc and argv
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

int QB_var_exists(struct Parser *parser, const char *name)
{
    struct VarEntry *entry = parser->vars;
    while(entry) {
        if(strcmp(entry->name, name) == 0) {
            return 1;
        }
        entry = entry->next;
    }
    return 0;
}

void QB_add_var(struct Parser *parser, const char *name)
{
    struct VarEntry *entry = malloc(sizeof(struct VarEntry));
    entry->name = strdup(name);
    entry->next = parser->vars;
    parser->vars = entry;
}

Node *QB_parse_variable_dec(struct Parser *parser)
{
    char *var_type = strndup(parser->current_token.start, parser->current_token.len);
    QB_parser_eat(parser, TOKEN_IDENTIFIER);

    if (!QB_parser_check(parser, TOKEN_IDENTIFIER)) {
        fprintf(stderr, "[ERROR]: Expected variable name after type '%s'\n", var_type);
        free(var_type);
        exit(EXIT_FAILURE);
    }

    char *var_name = strndup(parser->current_token.start, parser->current_token.len);

    if(QB_var_exists(parser, var_name)) {
        fprintf(stderr, "[ERROR]: Variable name have been declared twice in %s'\n", var_name);
        free(var_name);
        free(var_type);
        exit(EXIT_FAILURE);
    }

    Node *var_node = malloc(sizeof(Node));
    var_node->type = QB_VARIABLE;
    var_node->variable_dec_node.name = var_name;
    var_node->variable_dec_node.value = 0;

    if (QB_parser_check(parser, TOKEN_EQ)) {
        QB_parser_eat(parser, TOKEN_EQ);
        // TODO: Parse the expression/value
        Node *init_expr = QB_parser_expression(parser);

        if (init_expr->type == NODE_NUMBER) {
            var_node->variable_dec_node.value = init_expr->number_value;
        }
    }

    if (QB_parser_check(parser, TOKEN_SEMICOLON)) {
        QB_parser_eat(parser, TOKEN_SEMICOLON);
    }

    QB_add_var(parser, var_name);
    free(var_type);
    return var_node;
}

Node *QB_parser_assigment(struct Parser *parser)
{
    char *var_name = strndup(parser->current_token.start, parser->current_token.len);
    QB_parser_eat(parser, TOKEN_IDENTIFIER);

    QB_parser_eat(parser, TOKEN_EQ);

    Node *value = QB_parser_expression(parser);

    if (QB_parser_check(parser, TOKEN_SEMICOLON)) {
        QB_parser_eat(parser, TOKEN_SEMICOLON);
    }

    Node *assign_node = malloc(sizeof(Node));
    assign_node->type = NODE_ASSIGNMENT;
    assign_node->assignment_node.name = var_name;
    assign_node->assignment_node.value = value;

    return assign_node;
}
