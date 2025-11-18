#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "qb_lexer.h"
#include "qb_parser.h"
#include "qb_da.h"

#define BUF_SIZE 1024

static const char* TOKEN_NAMES[] = {
    "TOKEN_IDENTIFIER",
    "TOKEN_LARROW",
    "TOKEN_EOF",
    "TOKEN_EQ",
    "TOKEN_SEMICOLON",
    "TOKEN_COLON",
    "TOKEN_NUMBER",
    "TOKEN_PLUS",
    "TOKEN_SUBTRACT",
    "TOKEN_LBRACE",
    "TOKEN_RBRACE",
    "TOKEN_ASTERIK",
    "TOKEN_LESS",
    "TOKEN_GREATER",
    "TOKEN_DOUBLE_QUOTES",
};

void print_ast(Node *node, int indent) {
    if (!node) {
        for (int i = 0; i < indent; i++) printf("  ");
        printf("(null)\n");
        return;
    }

    for (int i = 0; i < indent; i++) printf("  ");

    switch (node->type) {
        case QB_FUNCTION:
            printf("Function: '%s'\n", node->function_node.name);
            for (int i = 0; i < indent; i++) printf("  ");
            printf("  Body:\n");
            print_ast(node->function_node.body, indent + 2);
            break;

        case NODE_BLOCK:
            printf("Block (statements: %d)\n", node->block_node.count);
            for (int i = 0; i < node->block_node.count; i++) {
                print_ast(node->block_node.statements[i], indent + 1);
            }
            break;
        // TODO: implement more stuff
        default:
            printf("Unknown node type: %d\n", node->type);
            break;
    }
}

void print_tokens(const char *source) {
    printf("\n=== TOKENS ===\n");
    struct Lexer lexer;
    QB_init_lexer(&lexer, source);

    struct Token token = {0};
    int token_count = 0;

    while (1) {
        token = QB_match_token(&lexer, &token);
        printf("Token %2d: %-20s | '%.*s'\n",
               ++token_count,
               TOKEN_NAMES[token.type],
               token.len,
               token.start);

        if (token.type == TOKEN_EOF) break;
    }
    printf("==============\n\n");
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "[ERROR]: Provide source file as argument\n");
        fprintf(stderr, "Usage: %s <source_file.qb>\n", argv[0]);
        return 1;
    }

    FILE *fptr = fopen(argv[1], "r");
    if (!fptr) {
        fprintf(stderr, "[ERROR]: Could not read file '%s'\n", argv[1]);
        return 1;
    }

    struct String_Builder sb = {0};
    char buf[BUF_SIZE];
    while (fgets(buf, sizeof(buf), fptr)) {
        sb_append(&sb, buf);
    }
    fclose(fptr);

    if (sb.count == 0 || sb.items[sb.count - 1] != '\0') {
        sb_append(&sb, "");
    }

    printf("=== SOURCE CODE ===\n");
    printf("%s", sb.items);
    printf("===================\n");

    print_tokens(sb.items);

    printf("=== PARSING ===\n");
    struct Parser parser;
    QB_parser_init(&parser, sb.items);

    Node *ast = NULL;

    if (QB_parser_check(&parser, TOKEN_IDENTIFIER)) {
        printf("Parsing function declaration");
        ast = QB_parse_funtion_dec(&parser);
        printf("Parse successful\n\n");
    } else {
        printf("Expected function declaration, got token type: %s\n",
               TOKEN_NAMES[parser.current_token.type]);
    }

    if (ast) {
        printf("=== ABSTRACT SYNTAX TREE ===\n");
        print_ast(ast, 0);
        printf("============================\n");
    } else {
        printf("No AST generated\n");
    }

    free(sb.items);

    return 0;
}
