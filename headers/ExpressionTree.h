#ifndef __EXPRESSION_TREE__
#define __EXPRESSION_TREE__

#include "tokenizer.h"
#include "stack.h"

// Inspired by https://github.com/PixelRifts/math-expr-evaluator/tree/master

typedef struct Tnode Tnode;
typedef Tnode *ExpressionTree;	// an expression tree is the memory address of a Tnode

// Tnode: a node in the parse tree (AST)
// 	- token: Token (have token string and length)
// - can either be a leaf node, unary operator node, or a binary operator node
// leaf: TOK_VAR | TOK_LIT | NULL
// internal: TOK_ADD | TOK_MINUS | TOK_MULT | TOK_DIV | TOK_MOD | TOK_INC | TOK_DEC
struct Tnode {
	enum tok_type_t type;
	long value;	// the (partial) result of the entire expression evaluated at this node 
			// (may use a float/double if decide to suport floating point values)
	union {
		struct { ExpressionTree operand; } unary;
		struct { ExpressionTree left; ExpressionTree right; } binary;
	};
}; 

#define NAN 0xffUL << 23 | 1	// a (bit) pattern resembling a not-a-number 32-bit float by IEEE-754


ExpressionTree expressiontree_build_tree(Token *stream, size_t length);
void expressiontree_print_to_file(FILE *fp, ExpressionTree root);
void expressiontree_destroy_tree(ExpressionTree *root);

#endif /* end of __EXPRESSION_TREE__ */
