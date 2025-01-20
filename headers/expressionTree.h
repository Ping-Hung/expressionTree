#ifndef __EXPRESSIONTREE_H__
#define __EXPRESSIONTREE_H__
#include "tokenizer.h"

typedef enum
{
    ADDMINUS     = 1,
    MULTDIV      = 2,
    LEFTPARENTH  = 3,
    RIGHTPARENTH = 4
} Precedence;

typedef struct
{
    char operator;
    Precedence pre;
} OperatorState;

typedef struct _ExpressionTreeNode
{
    TokensStr content;
    struct _ExpressionTreeNode *left;
    struct _ExpressionTreeNode *right;
} ExpressionTreeNode;

ExpressionTreeNode *build_tree(TokensStr *array, int start, int end);

#endif
