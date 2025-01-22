#ifndef __EXPRESSIONTREE_H__
#define __EXPRESSIONTREE_H__
#include "tokenizer.h"
#include "stack.h"

typedef struct _ExpressionTreeNode
{
    TokensStr content;
    struct _ExpressionTreeNode *left;
    struct _ExpressionTreeNode *right;
} ExpressionTreeNode;

ExpressionTreeNode *create_leaf_node(TokensStr content);
ExpressionTreeNode *create_internal_node(TokensStr content, ExpressionTreeNode *left, ExpressionTreeNode *right);
ExpressionTreeNode *build_tree(TokensStr *array, int n_tokens, StackFrame *operator_stack, StackFrame *output);

#endif
