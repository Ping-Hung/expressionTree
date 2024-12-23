#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "utils.h"
#include "expressionTree.h"

int main(int argc, char *argv[])
{
    // ask for input length
    int expectedLength = 0;
    char inputBuffer[4]; // assume at most 4 digit decimal number
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
    for (char ch = getchar(); (ch != EOF) && (ch != '\n'); ch = getchar());

    // receiving user input
    char *readResult = fgets(str, strLength, stdin);
    if (readResult == NULL)
    {
        fprintf(stderr, "something wrong happened when reading input");
        fprintf(stderr, "get %s\n", readResult);
        exit(EXIT_FAILURE);
    }
    int actualLength = count_acutal_length(str);
    fprintf(stdout, "str \"%s\" with length %d is entered, expected length = %d\n",
            str, actualLength, expectedLength);

    // build expressionTree

    // verify expressionTree

    // free string buffer
    free(str);
    return EXIT_SUCCESS;
}