#ifndef __EXPRESSION_TREE__
#define __EXPRESSION_TREE__
#include "tokenizer.h"

// Inspired by https://github.com/PixelRifts/math-expr-evaluator/tree/master

// using forward declaration to ease the effort of defining structure

typedef struct Tnode;
typedef Tnode *ExpressionTree;	// an expression tree is the memory address of a Tnode

// Tnode: a node in the parse tree (AST)
// 	- token: Token (have token string and length)
// - can either be a leaf node, unary operator node, or a binary operator node
struct Tnode {
	Token token;
	union {
		long result;	// the (partial) result of the entire expression evaluated at this node
		struct { ExpressionTree operand; } unary;
		struct { ExpressionTree left; ExpressionTree right; } binary;
	};
}; 


ExpressionTree expressiontree_build_tree(Token *stream, size_t length);
void expressiontree_print_to_file(FILE *fp, ExpressionTree root);
void expressiontree_destroy_tree(ExpressionTree *root);

#endif /* end of __EXPRESSION_TREE__ */
