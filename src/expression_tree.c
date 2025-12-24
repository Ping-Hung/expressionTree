#include "../headers/ExpressionTree.h"

typedef char precedence_t;

//  Unary('++'|'--') > Unary('+'|'-') > binary('*'|'/'|'%') > binary('+' | '-')

// Regardless of what expr actually looks like, expressiontree_build_tree will
// assume the rules/conventions of infix mathematical expressions and build a
// tree according to this grammar
//
// Grammar defines how to compose meaningful message from the atoms (basic building blocks) of a language
// a syntax tree is a structure build out of tokens which encodes these grammar rules

/* 		   Grammar:	(I guess writing them out gives me a picture of
 *  		   		 how the parsing process goes and what helpers come into
 *  		   		 play, but error handling is not specified in grammar...)
 *
 *     		   ops   := TOK_ADD | TOK_MINUS | TOK_MULT | TOK_DIV | TOK_MOD | TOK_INC | TOK_DEC
 *		   specialSymbols := TOK_ERROR | TOK_LPAREN | TOK_RPAREN
 *   		   atoms := TOK_VAR | TOK_LIT
 *
 *
 */

ExpressionTree expressiontree_build_tree(Token *expr, size_t length)
{
	// loop through token stream expr and build a tree if the expression is valid.

	// found a TOK_LPAREN ⇒  following expr.precedence += 1
	ExpressionTree root = NULL;
	// TOK_ERROR handling
	for (int i = 0; i < length - 1; i++) {

		// recursive build left

		// find precedence of valid token expr[i]
	}
	return root;
}

void expressiontree_print_to_file(FILE *fp, ExpressionTree root)
{
	return;
}

void expressiontree_destroy_tree(ExpressionTree *root)
{
	// freeing tree via a post order tree walk
	assert(root && "arg root must be a valid ExpressionTree (Tnode *)");
}

