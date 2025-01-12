#ifndef __EXPRESSIONTREE_H__
#define __EXPRESSIONTREE_H__
#include "tokenizer.h"

typedef struct _ExpressionTreeNode
{
    TokensStr content;
    struct _ExpressionTreeNode *left;
    struct _ExpressionTreeNode *right;
} ExpressionTreeNode;

ExpressionTreeNode *build_tree_from_string(Tokenizer *a_tkz, int index);

#endif
