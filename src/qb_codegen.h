#ifndef QB_CODEGEN_H
#define QB_CODEGEN_H

#include "qb_da.h"

// TODO: Implement more stuff
void generate_statement(Node *stmt, int *stack_offset, struct String_Builder *sb);
void generate_block(Node *block, struct String_Builder *sb);

#endif
