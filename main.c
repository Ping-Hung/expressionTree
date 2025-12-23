#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include "headers/tokenizer.h"

static long getline(char **lineptr, size_t *buff_size);

int main(void)
{
	char *str = NULL;
	size_t buff_size = 0;
	long input_size;

get_input:
	printf("enter an expression ending with a \";\": ");
	while ((input_size = getline(&str, &buff_size)) == -1) {
		if (getchar() == EOF) {
			fprintf(stdout, "\nentered EOF, program exit\n");
			free(str);
			return EXIT_FAILURE;
		}
		scanf("%*[^\n]");
		if (str[buff_size - 1] != ';') {
			fprintf(stdout, "input %*.s is too long for str of size %ld,"
					" building partial expressionTree\n",
					(int)buff_size, str, buff_size);
			goto build;
		}
		fprintf(stdout, "input failure, try again: ");
	}

	// handle incorrect format (**note input_size is length,
	// str[input_size - 1] is the last character
	if (str[input_size - 1] != ';') {
		fprintf(stdout, "expression \"%.*s\" is not terminated with a \";\"\n",
				(int)input_size, str);
		fprintf(stdout, "try again:\n");
		goto get_input;
	}

build:
	fprintf(stdout, "input expression: \"%.*s\"\nlength: %ld\n", 
		(int)input_size, str, input_size);
	// tokenize input string
	Tokenizer tkz = tokenizer_tokenize(str, input_size);

	tokenizer_display(&tkz);

	tokenizer_distroy(&tkz);
	free(str);
	return EXIT_SUCCESS;
}

#define BUF_DEFAULT 512
static long getline(char **lineptr, size_t *buff_size)
{
	/*
	 * - Successful read from stdin to *lineptr will return number of chars
	 *   read excluding '\0', the function will get rid of the '\n' on
	 *   successful reads.
	 * - Failures will return -1
	 * 	- Hitting EOF: *lineptr will contain n_read - 1 chars, 
	 * 	  *lineptr is *buff_size bytes long
	 * 	- Realloc failure: *lineptr will contain *buff_size - 1 chars,
	 * 	  *lineptr is *buff_size bytes long
	 *
	 * - getline is irresponsible for freeing memory, the caller must free
	 *   *lineptr no matter the result of getline
	 *  - *lineptr will be NULL terminated regardless
	 */
	assert(lineptr && "lineptr must be a valid char **");
	assert(buff_size && "buff_size must be a valid size_t *");
	if (*lineptr == NULL) {
		*buff_size = BUF_DEFAULT;
		*lineptr = malloc(sizeof(**lineptr) * (*buff_size));
	}

	int ch;
	long n_read = 0;
	for (ch = getchar(); ch != '\n' && ch != EOF; ch = getchar()) {
		if (n_read == *buff_size) {
			// *lineptr is too small, realloc
			void *buffer = realloc(*lineptr, sizeof(**lineptr) * (*buff_size) * 2);
			if (!buffer) {
				fprintf(stderr, "realloc failed in %s line %d\n",
						__FILE__, __LINE__ - 2);
				(*lineptr)[*buff_size] = '\0';
				return -1;
			}
			*lineptr = buffer;
			*buff_size *= 2;
		}
		(*lineptr)[n_read++] = ch;
	}

	(*lineptr)[n_read] = '\0';
	if (ch == EOF) {
		return -1;
	}
	return n_read;	// returning all char's read from stdin except '\0'
}
