#include <stdio.h>
#include <stdlib.h>
#include "qb_lexer.h"
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
};

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "[ERROR]: Provide source file as argument\n");
        return 1;
    }

    FILE *fptr = fopen(argv[1], "r");
    if (!fptr) {
        fprintf(stderr, "[ERROR]: Could not read file\n");
        return 1;
    }

    struct String_Builder sb = {0};

    char buf[BUF_SIZE];
    while (fgets(buf, sizeof(buf), fptr)) {
        sb_append(&sb, buf);
    }
    fclose(fptr);

    sb_append(&sb, "");

    struct Lexer lexer;
    QB_init_lexer(&lexer, sb.items);

    struct Token token = {0};

    while (1) {
        token = QB_match_token(&lexer, &token);

        printf("%-16s | '%.*s'\n", TOKEN_NAMES[token.type], token.len, token.start);

        if (token.type == TOKEN_EOF)
            break;
    }

    free(sb.items);

    return 0;
}
