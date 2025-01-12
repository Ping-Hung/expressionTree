#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "../headers/expressionTree.h"

inline bool _is_op(TokensStr str);

ExpressionTreeNode *build_tree(TokensStr *array, int start, int end)
{
    assert(array);
    /*
     * Algorithm outline:
     * 1. find binary operator and build a ExpressionTreeNode
     * 2. define the left subtree region in the array and the right subtree region of the array
     * 3. attach the left and right subtree onto the "BinOPNode" */
    if (start >= end)
    {
        return NULL;
    }
}

inline bool _is_op(TokensStr str)
{
    int length = strlen(str);
    if (length == 1)
    {
        return (str[0] == '+') || (str[0] == '-') || (str[0] == '*') || (str[0] == '/') || (str[0] == '%');
    }
    else if (length == 2)
    {
        return ((str[0] == '+') && (str[1] == '+')) || ((str[0] == '-') && (str[1] == '-'));
    }
    else
    {
        return false;
    }
}