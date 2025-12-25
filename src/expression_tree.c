#include "../headers/ExpressionTree.h"

//  Unary('++'|'--') > Unary('+'|'-') > binary('*'|'/'|'%') > binary('+' | '-')
typedef char precedence_t;

// static helpers
static inline int _expr_has_error(Token *expr, size_t length)
{
	// look for error tokens and track the number of '(' and ')'
	size_t n_lparen = 0, n_rparen = 0;
	for (Token *tok = expr; tok < expr + length; tok++) {
		switch (tok->type) {
		case TOK_ERROR:  return tok - expr;
		case TOK_LPAREN: n_lparen++; break;
		case TOK_RPAREN: n_rparen++; break;
		default: break;
		}
	}
	return (n_lparen == n_rparen) ? 0 : -1;
}


// Regardless of what expr actually looks like, expressiontree_build_tree will
// assume the rules/conventions of infix mathematical expressions and build a
// tree according to this grammar
//
// Grammar defines the structure of  meaningful message from the atoms (basic building blocks) of a language
// (i.e. the "shape" of a meaningful message)

// a syntax tree is nothing but tokens arranged in the grammatical structure

/* 		   Grammar:	
 *  		   (I guess writing them out gives me a picture of
 *  		    how the parsing process goes and what helpers come into
 *  		    play)
 *		   
 *		   mathExpr     := gropExpr op mathExpr
 *		   	        |  groupExpr
 *		   	        |  ε
 *     		   
 *     		   groupExpr    := TOK_LPAREN groupExpr TOK_RPAREN
 *			        |  expr
 *		   
 *		   expr         := primaryExpr TOK_ADD primaryExpr
 *		   	        |  primaryExpr TOK_MINUS primaryExpr
 *		   	        |  atom
 *
 *		   primaryExpr  := unaryExpr TOK_MULT unaryExpr
 *		   	        |  unaryExpr TOK_DIV unaryExpr
 *		   	        |  unaryExpr TOK_MOD unaryExpr
 *
 *		  unaryExpr     := prefixExpr
 *		  	        |  postfixExpr
 *		   
 *		  prefixExpr 	:= TOK_ADD atom
 *                              |  TOK_MINUS atom
 *                              |  TOK_INC atom
 *		  	        |  TOK_DEC atom
 *		  	        |  TOK_LIT TOK_VAR
 *		  	        |  atom
 * 		  
 * 		  postfixExpr   := atom INC
 *		  	        |  atom DEC
 *		  	        |  atom
 *
 *     		   op  		:= TOK_ADD | TOK_MINUS | TOK_MULT | TOK_DIV | 
 *     		       		   TOK_MOD | TOK_INC   | TOK_DEC
 *
 *   		   atom		:= TOK_VAR | TOK_LIT
 */

ExpressionTree expressiontree_build_tree(Token *expr, size_t length)
{
	// Do error and exception handling (return NULL when there is a lexing error)
	size_t error_idx = _expr_has_error(expr, length);

	char const *expr_str = expr[0].token_string;
	int expr_strlen = expr[length - 1].token_string + expr[length - 1].length 
		- expr_str;
	if (error_idx > 0) {
		fprintf(stdout, "error: mathematical expression %.*s is invalid\n",
				expr_strlen, expr_str);
		fprintf(stdout, "token[%ld]: %*.s is not a valid token\n", 
				error_idx, 
				(int)expr[error_idx].length,
				expr[error_idx].token_string);
		return NULL;
	}
	if (error_idx == -1) {
		fprintf(stdout, "error: unmatched number of \"(\" and \")\" in "
				"mathematical expression %.*s\n",
				(int)expr_strlen, 
				expr_str);
		return NULL;
	}


	// call statically defined recursive_descent to build actual tree
	
	ExpressionTree root = NULL;
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
	if (*root) {
		// recursively free the tree
	}
	*root = NULL;
}

