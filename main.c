#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include "headers/expressionTree.h"
#include "headers/tokenizer.h"

#define MAX_DECIMAL_DIGITS 4

static void _print_tkz(Tokenizer *a_tkz);
static void _print_tokens(char *tokens, int n_tokens);
static bool _valid_expr_format(char *str);
static void _free_list(StackFrame **a_list);

int main(int argc, char *argv[])
{
    // ask for input length
    int expectedLength = 0;
    char inputBuffer[MAX_DECIMAL_DIGITS];
    while (expectedLength <= 0)
    {
        fprintf(stdout, "enter expression length: ");
        fscanf(stdin, "%4s", inputBuffer); // MUST spcify exactly how many char read for memory safety
        expectedLength = strtol(inputBuffer, NULL, 10);
    }

    // alloc string buffer
    int strLength = 2 * expectedLength - 1;
    char *str = malloc(sizeof(*str) * strLength);

    // enter expression
    fprintf(stdout, "enter expression, end with \';\': ");

    // clear the input buffer (so no weird thing happning at fgets)
    for (char ch = getchar(); (ch != EOF) && (ch != '\n'); ch = getchar())
        ;

    // receiving user input
    while (fgets(str, strLength, stdin) == NULL || !_valid_expr_format(str))
    {
        fprintf(stderr, "something wrong happened when reading input, please try entering again\n");
    }

    fprintf(stderr, "%s\n", str);

    // tokenize input string
    Tokenizer tkz = init_tokenizer(str);
    tokenize(&tkz);
    _print_tkz(&tkz);

    // build expressionTree
    StackFrame *output = NULL;
    StackFrame *operator_stack = NULL;
    build_tree(tkz.array, tkz.n_tokens, operator_stack, output);

    // verify expressionTree

    // free tzk
    destroy_tokenizer(&tkz);

    // free stacks
    if (!operator_stack) {
        _free_list(&operator_stack);
    }
    if (!output) {
        _free_list(&operator_stack);
    }

    // free expressionTree

    return EXIT_SUCCESS;
}

static void _print_tkz(Tokenizer *a_tkz)
{
    if (a_tkz)
    {
        printf("tkz: \n");
        printf("{ \n");
        printf(" raw == %s", a_tkz->raw); // raw should include a newline
        printf(" length == %d\n", a_tkz->length);
        _print_tokens(a_tkz->tokens, a_tkz->numchar);
        printf(" numchar == %d\n", a_tkz->numchar);
        printf(" n_tokens == %d\n", a_tkz->n_tokens);
        for (int i = 0; i < a_tkz->n_tokens; ++i)
        {
            printf(" array[%d] == %s\n", i, a_tkz->array[i]);
        }
        printf("}\n");
    }
}

static void _print_tokens(char *tokens, int numchar)
{
    if (tokens)
    {
        printf(" tokens == ");
        for (int i = 0; i < numchar; ++i)
        {
            printf("%c", tokens[i]);
        }
        printf("\n");
    }
}

static bool _valid_expr_format(char *str)
{ // check for ';' at the end of the input expression
    char *a_ch = str;
    for (; *a_ch != '\0'; a_ch += 1)
        ;
    bool is_valid = *(a_ch - 2) == ';';
    if (!is_valid)
    {
        fprintf(stderr, "expression %s is not terminated with \";\"\n", str);
    }
    return is_valid;
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