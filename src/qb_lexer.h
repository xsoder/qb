#ifndef QB_LEXER_H
#define QB_LEXER_H

#include <stddef.h>

#define TOKEN \
    XX(TOKEN_IDENTIFIER)\
    XX(TOKEN_LARROW)\
    XX(TOKEN_EOF)\
    XX(TOKEN_EQ)\
    XX(TOKEN_SEMICOLON)\
    XX(TOKEN_COLON)\
    XX(TOKEN_NUMBER)\
    XX(TOKEN_PLUS)\
    XX(TOKEN_SUBTRACT)\
    XX(TOKEN_LBRACE)\
    XX(TOKEN_RBRACE)\
    XX(TOKEN_ASTERIK)\
    XX(TOKEN_LESS)\
    XX(TOKEN_GREATER)\
    XX(TOKEN_DOUBLE_QUOTES)\

enum TokenType {
    #define XX(value) value,
        TOKEN
    #undef XX
};

extern char *TokenName[];

struct Token {
    const char *name;
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
char QB_lexer_advance(struct Lexer *lexer);
char QB_lexer_peek(struct Lexer *lexer);
int  QB_is_alpha(char c);
int  QB_is_digit(char c);
void QB_skip_whitespace(struct Lexer *lexer);

// Token related function
struct Token QB_make_token(struct Lexer *lexer, struct Token *token, enum TokenType type);
struct Token QB_token_identifier(struct Lexer *lexer, struct Token *token);
struct Token QB_token_number(struct Lexer *lexer, struct Token *token);
struct Token QB_match_token(struct Lexer *lexer, struct Token *token);
char*  QB_get_token_name(enum TokenType type);

#endif //QB_LEXER_H
