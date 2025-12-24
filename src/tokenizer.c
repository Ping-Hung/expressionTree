#include "../headers/tokenizer.h"

static inline enum tok_type_t _assign_type(char ch);
static inline Token _group_chars_as_token(char const *start, char const *end);
static inline Token _tok_make_var(Token tok, char const *input_end);
static inline Token _tok_make_error(Token tok, char const *input_end);
static inline Token _tok_make_add_minus(Token tok);
static inline Token _tok_make_mult_div_mod(Token tok);

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
	while (input < input_end) {
		assert(i < length);

		while (input < input_end && isspace(*input)) {
			input++;
		}

		Token tok = _group_chars_as_token(input, input_end);
		n_tokens += 1;

		/* depending on the type of the first non-whitespace symbol, make
		 * different tokens accordingly, and write them into tokenizer.tokens[i]
		 */ 
		switch (tok.type) {
		// may continue consume symbol, type is determined
		case TOK_VAR: 
			tok = _tok_make_var(tok, input_end);
			break;
		case TOK_ERROR: 
			tok = _tok_make_error(tok, input_end);
			break;
		// stop consuming symbol, type can change based on result
		case TOK_ADD: case TOK_MINUS: 
			tok = _tok_make_add_minus(tok);
			break;
		case TOK_MULT: case TOK_DIV: case TOK_MOD:
			tok = _tok_make_mult_div_mod(tok);
			break;
		// below "ends" on the spot (i.e. no need to continue consuming, type is 
		// also determined)
		case TOK_LPAREN: case TOK_RPAREN: case TOK_LIT: case TOK_EOF: 
			break;
		default:
			break;
		}

		tokenizer.tokens[i] = tok;
		// loop update
		input =  tok.token_string + tok.length;
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
		// operators
		[TOK_ADD]     = "add operator",
		[TOK_MINUS]   = "minus operator",
		[TOK_MULT]    = "multiply operator",
		[TOK_DIV]     = "divide operator",
		[TOK_MOD]     = "modulus operator",
		[TOK_INC]     = "increment operator",
		[TOK_DEC]     = "decrement operator",
		// special symbols
		[TOK_LPAREN]  = "left parenthesis",
		[TOK_RPAREN]  = "right parenthesis",
		[TOK_ERROR]   = "invalid/error",
		[TOK_EOF]     = "end expression"
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

static inline enum tok_type_t _assign_type(char ch)
{
	if (isdigit(ch)) {
		return TOK_LIT;
	}
	if (isalpha(ch) || ch == '_') {
		return TOK_VAR;
	}
	switch (ch) {
	case '+': return TOK_ADD;
	case '-': return TOK_MINUS;
	case '*': return TOK_MULT;
	case '/': return TOK_DIV;
	case '%': return TOK_MOD;
	case '(': return TOK_LPAREN;
	case ')': return TOK_RPAREN;
	case '\0': return TOK_EOF;
	default:  return TOK_ERROR;
	}
}


static inline Token _group_chars_as_token(char const *start, char const *end)
{
	// group non-whitespace symbols of the same type into one unit (lexeme)
	assert(start && !isspace(*start) && 
		"start must be a valid non-whitespace char in the raw expression string");
	assert(end > start && "expression string begins at start must consist at least one char");

	char const *tok_end = start;
	enum tok_type_t start_type = _assign_type(*start);
	while (tok_end < end && !isspace(*tok_end) && _assign_type(*tok_end) == start_type) {
		tok_end++;
		if (start_type == TOK_LPAREN || start_type == TOK_RPAREN) {
			break;
		}
	}
	return (Token) {.token_string = start, .length = tok_end - start, .type = start_type};
}

static inline Token _tok_make_var(Token tok, char const *input_end)
{
	// while the first char after tok is of type literal, '_' followed by more literals,
	// continue consume input characters (expand the "width" of tok.token_string until
	// finds (OP|PAREN|EOF|ERROR)
	char const *tok_end = tok.token_string + tok.length;
	while (tok_end < input_end && 
	       (_assign_type(*tok_end) == TOK_LIT ||
		_assign_type(*tok_end) == TOK_VAR)) {
		tok_end++;
	}
	assert(_assign_type(*tok_end) != TOK_LIT && _assign_type(*tok_end) != TOK_VAR);
	tok.length = tok_end - tok.token_string;
	return tok;
}

static inline Token _tok_make_error(Token tok, char const *input_end)
{
	// an error token starts with an illegal symbol, followed by 0 or more 
	// non-whitespace symbols. I.e. TOK_ERROR := ^(ILLEGAL)+(.)*$
	char const *tok_end = tok.token_string + tok.length;
	while (tok_end < input_end && !isspace(*tok_end)) {
		tok_end++;
	}
	tok.length = tok_end - tok.token_string;
	return tok;
}

static inline Token _tok_make_add_minus(Token tok)
{
	// classify if a stream of '+' or '-' symbols are 
	// valid: (TOK_ADD|TOK_MINUS|TOK_INC|TOK_DEC) or invalid based
	// on tok.length
	switch (tok.length) {
	case 1:
		break;
	case 2: tok.type = (tok.type == TOK_ADD) ? TOK_INC : TOK_DEC;
		break;
	default: 
		tok.type = TOK_ERROR;
	}
	return tok;
}

static inline Token _tok_make_mult_div_mod(Token tok)
{
	if (tok.length > 1) {
		tok.type = TOK_ERROR;
	}
	return tok;
}
