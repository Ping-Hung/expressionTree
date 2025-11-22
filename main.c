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
static void _free_expressionTree(ExpressionTreeNode **a_root);
static void _print_tree(ExpressionTreeNode *root, FILE *fp, int depth);

int main(int argc, char *argv[])
{
	// ask for input length
	int expectedLength = 0;
	char inputBuffer[MAX_DECIMAL_DIGITS];
	while (expectedLength <= 0) {
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
	while (fgets(str, strLength, stdin) == NULL || !_valid_expr_format(str)) {
		fprintf(stderr, "something wrong happened when reading input, please try entering again\n");
	}

	fprintf(stderr, "%s\n", str);

	// tokenize input string
	Tokenizer tkz = init_tokenizer(str);
	tokenize(&tkz);
	_print_tkz(&tkz);

	// build expressionTree
	ExpressionTreeNode *root = build_tree(tkz.array, tkz.n_tokens);

	// verify expressionTree
	assert(root);
	FILE *fp = fopen("parseTree.txt", "w");
	if (fp) {
		_print_tree(root, fp, 0);
		fclose(fp);
		fprintf(stdout, "tree successfully printed to parseTree.txt\n");
	} else {
		fprintf(stderr, "failed to open file parseTree.txt\n");
	}

	// free tzk
	destroy_tokenizer(&tkz);

	// free expressionTree
	_free_expressionTree(&root);

	return EXIT_SUCCESS;
}

static void _print_tkz(Tokenizer *a_tkz)
{
	if (a_tkz) {
		printf("tkz: \n");
		printf("{ \n");
		printf(" raw == %s", a_tkz->raw); // raw should include a newline
		printf(" length == %d\n", a_tkz->length);
		_print_tokens(a_tkz->tokens, a_tkz->numchar);
		printf(" numchar == %d\n", a_tkz->numchar);
		printf(" n_tokens == %d\n", a_tkz->n_tokens);
		for (int i = 0; i < a_tkz->n_tokens; ++i) {
			printf(" array[%d] == %s\n", i, a_tkz->array[i]);
		}
		printf("}\n");
	}
}

static void _print_tokens(char *tokens, int numchar)
{
	if (tokens) {
		printf(" tokens == ");
		for (int i = 0; i < numchar; ++i) {
			printf("%c", tokens[i]);
		}
		printf("\n");
	}
}

static bool _valid_expr_format(char *str)
{ // check for ';' at the end of the input expression
	char *a_ch = str;
	for (; *a_ch != '\0'; a_ch += 1);
	bool is_valid = *(a_ch - 2) == ';';
	if (!is_valid) {
		fprintf(stderr, "expression %s is not terminated with \";\"\n", str);
	}
	return is_valid;
}

static void _free_expressionTree(ExpressionTreeNode **a_root)
{
	assert(a_root);
	if (*a_root) {
		_free_expressionTree(&((*a_root)->left));
		_free_expressionTree(&((*a_root)->right));
		free(*a_root);
	}
}

static void _print_tree(ExpressionTreeNode *root, FILE *fp, int depth)
{
	// print tree pointed to by root onto fp
	if (root) {
		for (int i = 0; i < depth; ++i) {
			fprintf(fp, "  ");
		}
		fprintf(fp, "%s\n", root->content);
		_print_tree(root->left, fp, depth + 1);
		_print_tree(root->right, fp, depth + 1);
	}
}
