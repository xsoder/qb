/*
The MIT License (MIT)
Copyright © 2025 xsoder

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*
For testing purpose when big migrate to nob.h by https://github.com/tsoding/nob.h. This is a helper header so you can have a genric way of writing any dynamic array. This is my attempt fo using a stripped down version of nob. This will also have features of bulding the project from scratch. I will fully make it a build system and have more functions implemented inside of it as well.
*/

#ifndef DA_H
#define DA_H
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define INIT_CAP 256

#define da_alloc(array) do {                                      \
    (array)->items = malloc(INIT_CAP * sizeof(*(array)->items));  \
    (array)->count = 0;                                           \
    (array)->capacity = INIT_CAP;                                 \
} while (0)

#define da_append(array, value) do {                                              \
    if ((array)->items == NULL || (array)->capacity == 0) {                       \
        da_alloc(array);                                                          \
    }                                                                             \
    if ((array)->count >= (array)->capacity) {                                    \
        (array)->capacity *= 2;                                                   \
        (array)->items = realloc((array)->items,                                  \
                                 (array)->capacity * sizeof(*(array)->items));    \
    }                                                                             \
    (array)->items[(array)->count++] = (value);                                   \
} while (0)

#define da_free(array) do {          \
    free((array)->items);            \
    (array)->items = NULL;           \
    (array)->count = 0;              \
    (array)->capacity = 0;           \
} while (0)

struct String_Builder {
    char *items;
    size_t capacity;
    size_t count;
};

static inline void sb_append(struct String_Builder *sb, const char *str)
{
    size_t len = strlen(str);
    if (sb->count + len + 1 > sb->capacity) {
        sb->capacity = (sb->count + len + 1) * 2;
        sb->items = realloc(sb->items, sb->capacity);
    }
    memcpy(sb->items + sb->count, str, len);
    sb->count += len;
    sb->items[sb->count] = '\0';
}

typedef struct Node Node;
struct Token;

typedef enum NodeType {
    NODE_NUMBER,
    NODE_BINARY_OP,
    NODE_BLOCK,
    QB_FUNCTION,
    QB_VARIABLE,
} NodeType;

typedef struct Node {
    enum NodeType type;
    union {
        int number_value;
        struct {
            Node *left;
            struct Token op;
            Node *right;
        } binary_op;
        struct {
            char *name;
            Node *body;
        } function_node;
        struct {
            char *name;
            int value;
        } variable_dec_node;
        struct {
            Node **statements;
            int count;
        } block_node;
    };
} Node;

static inline Node* create_number_node(int value)
{
    Node *node = malloc(sizeof(Node));
    node->type = NODE_NUMBER;
    node->number_value = value;
    return node;
}

static inline Node* create_binary_op_node(Node *left, struct Token *op, Node *right)
{
    Node *node = malloc(sizeof(Node));
    node->type = NODE_BINARY_OP;
    node->binary_op.left = left;
    node->binary_op.op = *op;
    node->binary_op.right = right;
    return node;
}

#endif // DA_H
