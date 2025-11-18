#include "qb_lexer.h"
#include <stdio.h>

void QB_init_lexer(struct Lexer *lexer, const char* src)
{
    lexer->start = src;
    lexer->current = src;
}


int QB_is_alpha(char c)
{
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           c == '_';
}

int QB_is_digit(char c)
{
    return (c >= '0' && c <= '9');
}


char QB_lexer_advance(struct Lexer *lexer)
{
    return *lexer->current++;
}

char QB_lexer_peek(struct Lexer *lexer)
{
    return *lexer->current;
}

void QB_skip_whitespace(struct Lexer *lexer)
{
    while(*lexer->current == ' ' || *lexer->current == '\t' || *lexer->current == '\n' || *lexer->current == '\r') {
        lexer->current++;
    }
}

struct Token QB_make_token(struct Lexer *lexer, struct Token *token, enum TokenType type) {
    struct Token current_token = *token;
    current_token.type = type;
    current_token.start = lexer->start;
    current_token.len = (int)(lexer->current - lexer->start);
    return current_token;
}

struct Token QB_token_identifier(struct Lexer *lexer, struct Token *token) {
    while (QB_is_alpha(QB_lexer_peek(lexer)) || QB_is_digit(QB_lexer_peek(lexer))) QB_lexer_advance(lexer);
    return QB_make_token(lexer, token, TOKEN_IDENTIFIER);
}

struct Token QB_token_number(struct Lexer *lexer, struct Token *token) {
    while (QB_is_digit(QB_lexer_peek(lexer))) QB_lexer_advance(lexer);
    return QB_make_token(lexer, token, TOKEN_NUMBER);
}

struct Token QB_match_token(struct Lexer *lexer, struct Token *token)
{
    struct Token current_token = *token;
    QB_skip_whitespace(lexer);
    lexer->start = lexer->current;

    char c = QB_lexer_advance(lexer);
    if (QB_is_alpha(c)) return QB_token_identifier(lexer, &current_token);
    if (QB_is_digit(c)) return QB_token_number(lexer, &current_token);

    if(c == '-'){
        c = QB_lexer_advance(lexer);
        if(c == '>') {
            return QB_make_token(lexer, &current_token, TOKEN_LARROW);
        } else {
            return QB_make_token(lexer, &current_token, TOKEN_SUBTRACT);
        }
    }
    // TODO: Single symbol token
    switch (c) {
        case ':'    : return QB_make_token(lexer, &current_token, TOKEN_COLON);
        case '+'    : return QB_make_token(lexer, &current_token, TOKEN_PLUS);
        case ';'    : return QB_make_token(lexer, &current_token, TOKEN_SEMICOLON);
        case '{'    : return QB_make_token(lexer, &current_token, TOKEN_LBRACE);
        case '}'    : return QB_make_token(lexer, &current_token, TOKEN_RBRACE);
        case '='    : return QB_make_token(lexer, &current_token, TOKEN_EQ);
        case '*'    : return QB_make_token(lexer, &current_token, TOKEN_ASTERIK);
        case '<'    : return QB_make_token(lexer, &current_token, TOKEN_LESS);
        case '>'    : return QB_make_token(lexer, &current_token, TOKEN_GREATER);
        case '"'    : return QB_make_token(lexer, &current_token, TOKEN_DOUBLE_QUOTES);
    }
    return QB_make_token(lexer, &current_token, TOKEN_EOF);
}
