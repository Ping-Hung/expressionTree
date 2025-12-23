#include "../headers/tokenizer.h"

static inline enum type_t _assign_type(char ch);
static inline enum type_t _classify_operator(char const *token_str, size_t length);

Tokenizer tokenizer_tokenize(char const *input, size_t length)
{
	assert(input && "argument input must be non-null");

	Tokenizer tokenizer = { 0 };
	// tokenizer.tokens is ought to be Token[length]
	tokenizer.tokens = malloc(sizeof(*(tokenizer.tokens)) * length);

	// use a sliding-window approach to isolate each token from input
	int i = 0;	// index of the tokens array
	size_t n_tokens = 0;
	char const *input_end = input + length;
	while (input != input_end) {
		assert(i < length);
		// eat white space
		while (isspace(*input)) {
			input++;
		}

		n_tokens += 1;

		/* assign a type to the first non-whitespace char and match
		 * until the first different char. 
		 * (i.e. expand the "token width" as far as possible) 
		 */
		enum type_t type = _assign_type(*input);
		char const *tok_end = input + 1;
		while (tok_end < input_end && _assign_type(*tok_end) == type) {
			tok_end++;
		}

		/* depending on the type of the different char, either
		 * 1. continue matching forward
		 * 2. contract to appropriate char/spot (cases of the parentheses)
		 * 3. end on the spot
		 */ 
		switch (type) {
		case TOK_OP:	/* end on the spot */
			type = _classify_operator(input, tok_end - input);
			break;
		case TOK_VAR:	/* continue matching */
			{
				while (tok_end < input_end && 
				       (_assign_type(*tok_end) == TOK_LIT || 
					_assign_type(*tok_end) == TOK_VAR)) {
					tok_end++;
				}
			}
			break;
		case TOK_LPAREN: /* contract to appropriate spot */
		case TOK_RPAREN:
				tok_end = input + 1;
				break;
		default:	/* end on the spot */
			break;
		}

		tokenizer.tokens[i] = (Token) {
			.token_string = input,
			.length = tok_end - input,
			.type = type
		};

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
		[TOK_VAR]     = "variable",
		[TOK_LIT]     = "literal",
		[TOK_OP]      = "operator",
		[TOK_LPAREN]  = "left parenthesis",
		[TOK_RPAREN]  = "right parenthesis",
		[TOK_INVALID] = "invalid"
	};

	printf("tokenizer:\n");
	printf("{\n");
	printf("  .n_tokens: %ld\n", a_tkz->n_tokens);

	for (int i = 0; i < a_tkz->n_tokens; i++) {
		printf("  .tokens[%d] = {\n", i);
		// print a_tkz->tokens[i].length characters in a_tkz->tokens[i].token_string
		printf("  		   .token_string = \"%.*s\"\n"
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

static inline enum type_t _assign_type(char ch)
{
	if (isdigit(ch)) {
		return TOK_LIT;
	}
	if (isalpha(ch) || ch == '_') {
		return TOK_VAR;
	}
	switch (ch) {
	case '+': case '-': case '*': case '/': case '%':
		return TOK_OP;
	case '(':
		return TOK_LPAREN;
	case ')':
		return TOK_RPAREN;
	default:
		break;
	}
	return TOK_INVALID;
}

static inline enum type_t _classify_operator(char const *token_str, size_t length)
{
	// determine if a stream of operators is a valid operator
	switch (length) {
	case 1:
		return TOK_OP;
	case 2:
		{
			if ((token_str[0] == token_str[1]) && 
			    (token_str[0] == '+' || token_str[0] == '-')) {
				return TOK_OP;
			} else {
				return TOK_INVALID;
			}
		}
	default:
		return TOK_INVALID;
	}
}

