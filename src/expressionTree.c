#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "../headers/expressionTree.h"

inline bool _is_op(TokensStr str);

ExpressionTreeNode *build_tree(TokensStr *array, int start, int end)
{ // * Look into "shunting-yard algorithm" by Dijkstra
    assert(array);
    return NULL;
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