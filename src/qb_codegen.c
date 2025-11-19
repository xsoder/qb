#include "qb_codegen.h"
#include <stdio.h>

void generate_statement(Node *stmt, int *stack_offset, struct String_Builder *sb)
{
    if (!stmt) return;

    char buffer[256];

    switch (stmt->type) {
        case QB_VARIABLE:
            *stack_offset += 4;
            sb_append(sb, "    sub rsp, 4\n");
            if (stmt->variable_dec_node.value != 0) {
                snprintf(buffer, sizeof(buffer), "    mov dword [rbp-%d], %d\n",
                       *stack_offset, stmt->variable_dec_node.value);
                sb_append(sb, buffer);
            }
            break;

        case NODE_ASSIGNMENT:
            // TODO: Assignment
            snprintf(buffer, sizeof(buffer), "    ; Assignment to '%s' Unimplemnted\n",
                   stmt->assignment_node.name);
            sb_append(sb, buffer);
            break;

        case NODE_FUNCTION_CALL:
            sb_append(sb, "    ; Function call\n");
            break;

        default:
            snprintf(buffer, sizeof(buffer), "    ; Unknown statement type %d\n", stmt->type);
            sb_append(sb, buffer);
            break;
    }
}

void generate_block(Node *block, struct String_Builder *sb)
{
    if (!block || block->type != NODE_BLOCK) return;

    int stack_offset = 0;

    for (int i = 0; i < block->block_node.count; i++) {
        generate_statement(block->block_node.statements[i], &stack_offset, sb);
    }

    if (stack_offset > 0) {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "    add rsp, %d\n", stack_offset);
        sb_append(sb, buffer);
    }
}
