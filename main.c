#include <stdio.h>
#include <stdlib.h>
#include "expressionTree.h"

int main(int argc, char *argv[]) 
{
    //ask for input length
    int expressionLength = 0;
    char inputBuffer[4];    //assume at most 4 digit decimal number

    while (expressionLength <= 0)
    {
        fprintf(stdout, "enter expression length: ");
        fscanf(stdin, "%4s", inputBuffer); // MUST specify exactly how many char read for memory safety
        expressionLength = strtol(inputBuffer, NULL, 10);
    }
    
    //alloc string buffer
    char *str = malloc(sizeof(*str) * expressionLength);
    
    fprintf(stdout, "str \"%s\" with length %d is entered\n", str, expressionLength);
    //build expressionTree

    //verify expressionTree


    //free string buffer
    free(str);
    return EXIT_SUCCESS;
}