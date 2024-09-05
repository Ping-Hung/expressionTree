#ifndef __EXPRESSIONTREE_H__
#define __EXPRESSIONTREE_H__

typedef struct _ExpressionTreeNode
{
    char content;
    struct _ExpressionTreeNode *left;
    struct _ExpressionTreeNode *right;
} ExpressionTreeNode;

ExpressionTreeNode *build_tree_from_string(char const *str, int index);

#endif


