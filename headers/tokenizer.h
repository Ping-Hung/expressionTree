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
 *   following tok_type_t and filtering out whitespaces.
 *
 * Parser will take this stream of tokens and build
 * structure (ExpresssionTree) based on the "grammatical" rules of computer
 * arithmetic.
 */

enum tok_type_t {
	// atoms
	TOK_VAR, TOK_LIT,
	// operators: just a bunch of reserved symbols that meant math operations
	//  (what kind of operation will be determined in the parsing/tree-building stage)
	TOK_ADD,TOK_MINUS,TOK_MULT, TOK_DIV, TOK_MOD,
	// unary operators
	TOK_INC, TOK_DEC,
	// special symbols (parentheses meant grouping, EOF meant end of expression)
	TOK_LPAREN,TOK_RPAREN, TOK_EOF,
	// error type: none of the above
	// an TOK_ERROR starts with an illegal character followed by 0 or more
	// non-whitespace characters
	TOK_ERROR
};

// Token:
// 	- token_string: char * := starting address of the token
// 	- length: size_t := length of the token 
// 	- type: enum tok_type_t:= what kind of token 
// 	** want token_string[0...length -  1] to entail all the characters in the actual token **
typedef struct {
	enum tok_type_t type;
	char const *token_string;
	size_t length;
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
