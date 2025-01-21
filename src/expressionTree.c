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

ExpressionTreeNode *create_internal_node(TokensStr content, ExpressionTreeNode *left, ExpressionTreeNode *right)
{
    // should ensure left and right are non-NULL
    assert(left);
    assert(right);

    ExpressionTreeNode *new = malloc(sizeof(*new));
    if (!new)
    {
        fprintf(stderr, "%s:%s: malloc failed\n", __FILE__, __func__);
        return NULL;
    }
    _copy_str(new->content, content);
    new->left = left;
    new->right = right;
    return new;
}

// side note: TokenStr <=> char [256] ---maybe I should've stayed with good old char * instead of smart assing myself
ExpressionTreeNode *build_tree(TokensStr *array, int n_tokens)
{ /*  Modified "shunting-yard algorithm" suggested by ChatGPT */
    assert(array);

    // setup for the algorithm
    _init_precedenceTable();
    // two stacks required for the algorithm
    StackFrame *operator_stack = NULL;
    StackFrame *output = NULL;

    // examine each token in array
    for (size_t i = 0; i < n_tokens; ++i)
    {
        TokensStr *token = array[i];

        if (_is_op(*token))
        {
            // While the operator at the top of the operator stack has higher or equal precedence than token
            // pop two nodes from the output stack to form the left and right children of a new
            // ExpressionTreeNode
            while (!is_empty(&operator_stack))
            {
                TokensStr *top = get_top(&operator_stack); // reminder: top is of type char[1024]
                if (precedanceTable[(int)(*top)] >= precedanceTable[(int)(*token)])
                {
                    pop(&operator_stack);
                    ExpressionTreeNode *left = get_top(&output);
                    ExpressionTreeNode *right = get_top(&output);
                    pop(&output);
                    pop(&output);
                    create_internal_node(*top, left, right);
                }
            }
        }
        else
        { // gettting VARiables and LITerals
            /*  based on the design of tokenizer, if the first char in str is a number, the entire string is a number
                likewise for variables */
            push(&output, create_leaf_node(*token));
            // should ensure the contents output are all ExpressionTreeNode malloc'd
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
    precedanceTable[(int)'+'] = 1;
    precedanceTable[(int)'-'] = 1;
    precedanceTable[(int)'*'] = 2;
    precedanceTable[(int)'/'] = 2;
    precedanceTable[(int)'%'] = 2;
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