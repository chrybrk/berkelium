#include "include/interpret.h"
#include "include/macro.h"

int interpret(struct ASTnode *node)
{
    int left, right;

    if (node->left) left = interpret(node->left);
    if (node->right) right = interpret(node->right);

    switch (node->op)
    {
        case AST_ADD: return (left + right);
        case AST_SUB: return (left - right);
        case AST_MUL: return (left * right);
        case AST_DIV: return (left / right);
        case AST_INTLIT: return (node->intvalue);
        default: log(3, "%s", "Unrecognised ASTnode:Type");
    }

    return 0;
}
