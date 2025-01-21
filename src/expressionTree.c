#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include "../headers/expressionTree.h"

/*An LUT to look up precendence of operator in constant time. Default all values to -1 */
int8_t precedanceTable[256] = {-1};

inline bool _is_op(TokensStr str);
inline void _init_precedenceTable();
static void _copy_str(char *dest, char *src);

ExpressionTreeNode *create_leaf_node(TokensStr content)
{
    // must ensure content is a literal, else wrong
    ExpressionTreeNode *leaf = malloc(sizeof(*leaf));
    _copy_str(leaf->content, content);
    leaf->left = NULL;
    leaf->right = NULL;
    return leaf;
}
ExpressionTreeNode *create_internal_node(TokensStr content, ExpressionTreeNode *left, ExpressionTreeNode *right);

// side note: TokenStr <=> char [256] ---maybe I should've stayed with good old char * instead of smart assing myself
ExpressionTreeNode *build_tree(TokensStr *array, int n_tokens)
{ /*  Modified "shunting-yard algorithm" suggested by ChatGPT */
    assert(array);

    // two stacks required for the algorithm
    StackFrame *operator_stack = NULL;
    StackFrame *output = NULL;

    // for token in tokens:.....
    for (size_t i = 0; i < n_tokens; ++i)
    {
        TokensStr *a_str = array[i];

        if (_is_op(*a_str))
        { /* get an operator */
            // TODO: figure out the operator while loop logic\
            // let's not worried about "()" for now
        }
        else
        { // gettting VARiables and LITerals
          // based on the design of tokenizer, if the first char in str is a number, the entire string is a number
          // likewise for variables
            push(&output, create_leaf_node(*a_str));
        }
    }

    return NULL;
}

inline bool _is_op(TokensStr str)
{
    if (strlen(str) == 1)
    {
        return (str[0] == '+') || (str[0] == '-') || (str[0] == '*') || (str[0] == '/') || (str[0] == '%');
    }
    return false;
}

inline void _init_precedenceTable()
{
    // higher the precendence, the sooner the evaluation
    precedanceTable[(uint8_t)'+'] = 1;
    precedanceTable[(uint8_t)'+'] = 1;
    precedanceTable[(uint8_t)'*'] = 2;
    precedanceTable[(uint8_t)'/'] = 2;
    precedanceTable[(uint8_t)'%'] = 2;
}

static void _copy_str(char *dest, char *src)
{
    assert(dest);
    assert(src);

    int i = 0;
    for (char *a_char = src; *a_char != '\0'; ++a_char)
    {
        dest[i] = *a_char;
    }
    dest[i + 1] = '\0';
}