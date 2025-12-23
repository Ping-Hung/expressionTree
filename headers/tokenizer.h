#ifndef __TOKENIZER_H__
#define __TOKENIZER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <ctype.h>

/*
 * lexer/tokenizer takes an input character stream and returns a stream of tokens.
 * - It is done here by labelling substrings of the input stream as one of the
 *   following type_t and filtering out whitespaces.
 *
 * The parser (non-existent unit) will take this stream of tokens and build
 * structure (ExpresssionTree) based on the "grammatical" rules of computer
 * arithmetic.
 */

enum type_t {
	TOK_VAR,
	TOK_LIT,
	TOK_OP,
	TOK_LPAREN,
	TOK_RPAREN,
	TOK_INVALID
};

// Token:
// 	- token_string: char * := starting address of the token
// 	- length: size_t := length of the token 
// 	- type: enum type_t:= what kind of token 
// 	** want token_string[0...length -  1] to entail all the characters in the actual token **
typedef struct {
	char const *token_string;
	size_t length;
	enum type_t type;
} Token;


/* Tokenizer:  
 *	-  tokens: Token[] := an array of Token
 *	-  n_tokens: size_t := length of tokens (n_tokens ≤ input_size)
 */
typedef struct {
	Token *tokens;
	size_t n_tokens;
} Tokenizer;

Tokenizer tokenizer_tokenize(char const *input, size_t length);
void tokenizer_display(Tokenizer *a_tkz);
void tokenizer_distroy(Tokenizer *a_tkz);	// free the tokens array basically

#endif
