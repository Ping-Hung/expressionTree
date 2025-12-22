#include "../headers/tokenizer.h"

static inline enum type_t _assign_type(char ch)
{
	if (isdigit(ch)) {
		return TOK_LIT;
	}
	if (isalpha(ch)) {
		return TOK_VAR;
	}
	switch (ch) {
	case '+':
	case '-':
	case '*':
	case '/':
	case '%':
		return TOK_OP;
	default:
		break;
	}
	return TOK_INVALID;
}

Tokenizer tokenizer_tokenize(char const *input, size_t length)
{
	assert(input && "argument input must be non-null");

	Tokenizer tokenizer = { 0 };
	// tokenizer.tokens is ought to be Token[length]
	tokenizer.tokens = malloc(sizeof(*(tokenizer.tokens)) * length);

	// use a sliding-window approach to isolate each token from input
	int i = 0;			// index to the tokens array
	size_t n_tokens = 0;
	char const *input_end = &input[length];
	while (input != input_end) {
		assert(i < length);
		// eat white space
		while (isspace(*input)) {
			input++;
		}
		// classify the first non-whitespace character
		tokenizer.tokens[i].type = _assign_type(*input);
		n_tokens += 1;
		// finding the end of this token (the first character with different type)
		char const *tok_end = input;
		while (tok_end != input_end && 
			_assign_type(*tok_end) == tokenizer.tokens[i].type) {
			tok_end++;
		}

		tokenizer.tokens[i].token_string = input;
		tokenizer.tokens[i].length = tok_end - input;
		
		// loop update
		input = tok_end;
		i++;
	}

	tokenizer.n_tokens = n_tokens;
	return tokenizer;
}



void tokenizer_display(Tokenizer *a_tkz)
{
	assert(a_tkz && "parameter a_tkz must be non-NULL");

	char *tok_types[] = {
		[TOK_VAR] = "variable",
		[TOK_LIT] = "literal",
		[TOK_OP] = "operator",
		[TOK_INVALID] = "invalid"
	};

	printf("tokenizer:\n");
	printf("{\n");
	printf("  .n_tokens: %ld\n", a_tkz->n_tokens);

	for (int i = 0; i < a_tkz->n_tokens; i++) {
		printf("  .tokens[%d] = {\n", i);
		// print a_tkz->tokens[i].length characters in a_tkz->tokens[i].token_string
		printf("  		   .token_string = %.*s\n"
		       "		   .length	 = %ld\n"
		       "		   .type 	 = %s\n"
		       "		}\n",
		       (int)a_tkz->tokens[i].length, a_tkz->tokens[i].token_string,
		       a_tkz->tokens[i].length,
		      tok_types[a_tkz->tokens[i].type]);
	}

	printf("}\n");
}

void tokenizer_distroy(Tokenizer *a_tkz)
{
	assert(a_tkz && "parameter a_tkz must be non-NULL");
	free(a_tkz->tokens);
}

