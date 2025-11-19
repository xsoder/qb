#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "qb_lexer.h"
#include "qb_parser.h"
#include "qb_codegen.h"
#include "qb_da.h"

#define BUF_SIZE 1024

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
    struct String_Builder file_sb = {0};
    char buf[BUF_SIZE];
    while (fgets(buf, sizeof(buf), fptr)) {
        sb_append(&sb, buf);
    }
    fclose(fptr);

    if (sb.count == 0 || sb.items[sb.count - 1] != '\0') {
        sb_append(&sb, "");
    }

    struct Parser parser;
    QB_parser_init(&parser, sb.items);

    Node *node = NULL;
    if(QB_parser_check(&parser, TOKEN_IDENTIFIER)) {
        struct Lexer saved = parser.lexer;
        struct Token saved_tok = parser.current_token;

        QB_parser_advance(&parser);

        if (QB_parser_check(&parser, TOKEN_COLON)) {
            QB_parser_eat(&parser, TOKEN_COLON);
            if (QB_parser_check(&parser, TOKEN_COLON)) {
                parser.lexer = saved;
                parser.current_token = saved_tok;
                node = QB_parse_funtion_dec(&parser);
            } else {
                fprintf(stderr, "Syntax error: single ':' unexpected.\n");
                exit(1);
            }
        } else {
            parser.lexer = saved;
            parser.current_token = saved_tok;
            node = QB_parser_expression(&parser);
        }
    }

    sb_append(&file_sb, "format ELF64\n");
    sb_append(&file_sb, "section '.text' executable\n");

    if (node) {
        if (node->type == QB_FUNCTION) {
            char buffer[256];
            snprintf(buffer, sizeof(buffer), "public %s\n", node->function_node.name);
            sb_append(&file_sb, buffer);

            snprintf(buffer, sizeof(buffer), "%s:\n", node->function_node.name);
            sb_append(&file_sb, buffer);

            sb_append(&file_sb,"    push rbp\n");
            sb_append(&file_sb,"    mov rbp, rsp\n");

            generate_block(node->function_node.body, &file_sb);

            sb_append(&file_sb,"    mov rsp, rbp\n");
            sb_append(&file_sb,"    pop rbp\n");
            sb_append(&file_sb,"    ret\n");

        } else if (node->type == NODE_IDENTIFIER) {
            printf("public %s\n", node->identifer_node.name);
        } else if (node->type == QB_VARIABLE) {
            printf("public %s\n", node->variable_dec_node.name);
        }
    }

    char output_filename[256];
    snprintf(output_filename, sizeof(output_filename), "%s.asm", argv[1]);
    FILE *file = fopen(output_filename, "w");
    if (!file) {
        fprintf(stderr, "[ERROR]: Could not write to file '%s'\n", output_filename);
        free(node);
        free(sb.items);
        free(file_sb.items);
        return 1;
    }

    fprintf(file, "%s", file_sb.items);
    fclose(file);
    return 0;
}
