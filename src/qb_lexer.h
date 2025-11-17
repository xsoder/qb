#ifndef QB_LEXER_H
#define QB_LEXER_H

#include <stddef.h>

enum TokenType {
    TOKEN_IDENTIFIER,
    TOKEN_LARROW,
    TOKEN_EOF,
    TOKEN_EQ,
    TOKEN_SEMICOLON,
    TOKEN_COLON,
    TOKEN_NUMBER,
    TOKEN_PLUS,
    TOKEN_SUBTRACT,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_ASTERIK,
};

struct Token {
    enum TokenType type;
    const char *start;
    int len;
};

struct Lexer {
    const char* start;
    const char* current;
};

// Helper functions
void QB_init_lexer(struct Lexer *lexer, const char* src);
char QB_advance(struct Lexer *lexer);
char QB_peek(struct Lexer *lexer);
int  QB_is_alpha(char c);
int  QB_is_digit(char c);
void QB_skip_whitespace(struct Lexer *lexer);

// Token related function
struct Token QB_make_token(struct Lexer *lexer, struct Token *token, enum TokenType type);
struct Token QB_toekn_identifier(struct Lexer *lexer, struct Token *token);
struct Token QB_token_number(struct Lexer *lexer, struct Token *token);
struct Token QB_match_token(struct Lexer *lexer, struct Token *token);

#endif //QB_LEXER_H
