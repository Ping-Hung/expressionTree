#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "headers/expressionTree.h"
#include "headers/tokenizer.h"

#define MAX_DECIMAL_DIGITS 4

static void _print_tkz(Tokenizer *a_tkz);
static void _print_tokens(char *tokens, int n_tokens);

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
    if (fgets(str, strLength, stdin) == NULL)
    {
        fprintf(stderr, "something wrong happened when reading input");
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "%s\n", str);

    // tokenize input string
    Tokenizer tkz = init_tokenizer(str);
    tokenize(&tkz);
    _print_tkz(&tkz);
    // build expressionTree

    // verify expressionTree

    // free tzk
    destroy_tokenizer(&tkz);

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
        _print_tokens(a_tkz->tokens, a_tkz->n_tokens);
        printf(" n_tokens == %d\n", a_tkz->n_tokens);
        printf("}\n");
    }
}

static void _print_tokens(char *tokens, int n_tokens)
{
    if (tokens)
    {
        printf(" tokens == ");
        for (int i = 0; i < n_tokens; ++i)
        {
            printf("%c", tokens[i]);
        }
        printf("\n");
    }
}