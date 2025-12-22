#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include "headers/tokenizer.h"

#define MAX_EXPR_SIZE 1 << 14

int main(int argc, char *argv[])
{
	char *str = malloc(sizeof(*str) * MAX_EXPR_SIZE);
handle_input:
	fprintf(stdout, "enter an expression that ends with\";\": ");
	while (fgets(str, MAX_EXPR_SIZE, stdin) == NULL) {
		if (fgetc(stdin) == EOF) {
			printf("\nentered EOF, program exit\n");
			free(str);
			return EXIT_FAILURE;
		}
		scanf("%*[^\n]");
		fprintf(stdout, "try again: ");
	}
	
	// get rid of the '\n' and determine length
	size_t input_size = strlen(str);
	str[--input_size] = '\0';
	if (str[input_size - 1] != ';') {
		fprintf(stderr, "expression %s is not terminated with \";\"\n", str);
		goto handle_input;
	}

	fprintf(stderr, "input: %s, size: %ld\n", str, input_size);

	// tokenize input string
	Tokenizer tkz = tokenizer_tokenize(str, input_size);

	// today's goal is to refactor tokenizer properly
	tokenizer_display(&tkz);

	tokenizer_distroy(&tkz);
	free(str);
	return EXIT_SUCCESS;
}
