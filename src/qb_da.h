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
