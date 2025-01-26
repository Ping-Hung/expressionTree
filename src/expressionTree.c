#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include "../headers/expressionTree.h"

/*An LUT to look up precendence of operator in constant time. Default all values to -1 */
int8_t precedanceTable[256] = {-1};

static inline bool _is_op(TokensStr str);
static inline void _init_precedenceTable();
static void _copy_str(char *dest, char *src);
static void _free_list(StackFrame **a_list);

ExpressionTreeNode *create_leaf_node(TokensStr content)
{
    // must ensure content is a literal, else wrong
    ExpressionTreeNode *leaf = malloc(sizeof(*leaf));
    if (!leaf)
    {
        fprintf(stderr, "%s:%s: malloc failed\n", __FILE__, __func__);
        return NULL;
    }
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

// side note: TokenStr ⇔ char [1024], so TokenStr * ⇔ char (*)[1024] (address of an array of 1024 char)
ExpressionTreeNode *build_tree(TokensStr *array, int n_tokens)
{
    // Modified "shunting-yard algorithm" suggested by ChatGPT
    assert(array);
    // setup for the algorithm
    _init_precedenceTable();
    StackFrame *output = NULL;
    StackFrame *operator_stack = NULL;

    // examine each token in array
    for (size_t i = 0; i < n_tokens; ++i)
    {
        char *token = array[i];

        if (_is_op(token))
        {
            while (!is_empty(operator_stack))
            {
                char *top = get_top(operator_stack); // reminder: top is of type char[1024]
                if (precedanceTable[(int)(*top)] >= precedanceTable[(int)(*token)])
                {
                    pop(&operator_stack);

                    ExpressionTreeNode *right = get_top(output);
                    pop(&output);
                    ExpressionTreeNode *left = get_top(output);
                    pop(&output);

                    push(&output, create_internal_node(top, left, right));
                }
            }
            push(&operator_stack, token);
        }
        else
        {
            // gettting VARiables and LITerals
            push(&output, create_leaf_node(token));
        }
    }

    // building the entire tree
    while (!is_empty(operator_stack))
    {
        TokensStr *top = get_top(operator_stack);
        pop(&operator_stack);

        ExpressionTreeNode *right = get_top(output);
        pop(&output);
        ExpressionTreeNode *left = get_top(output);
        pop(&output);

        push(&output, create_internal_node(*top, left, right));
    }

    // it's garaunteed that one last treenode will be present on the output stack when the routine finishes
    ExpressionTreeNode *root = get_top(output);
    if (operator_stack)
    {
        _free_list(&operator_stack);
    }
    if (output)
    {
        _free_list(&output);
    }

    return root;
}

static inline bool _is_op(TokensStr str)
{
    if (strlen(str) == 1)
    {
        return (str[0] == '+') || (str[0] == '-') || (str[0] == '*') || (str[0] == '/') || (str[0] == '%');
    }
    return false;
}

static inline void _init_precedenceTable()
{
    // higher the precendence, the sooner the evaluation
    precedanceTable[(int8_t)'+'] = 1;
    precedanceTable[(int8_t)'-'] = 1;
    precedanceTable[(int8_t)'*'] = 2;
    precedanceTable[(int8_t)'/'] = 2;
    precedanceTable[(int8_t)'%'] = 2;

    precedanceTable[(int8_t)'('] = 0;
    // note the closing brace ')' is not included in the table
}

static void _copy_str(char *dest, char *src)
{
    assert(dest);
    assert(src);

    int i = 0;
    for (char *a_char = src; *a_char != '\0'; ++a_char)
    {
        dest[i++] = *a_char;
    }
    dest[i] = '\0';
}

static void _free_list(StackFrame **a_list)
{
    while (*a_list != NULL)
    {
        StackFrame *victim = *a_list;
        *a_list = (*a_list)->next;
        free(victim);
    }
}