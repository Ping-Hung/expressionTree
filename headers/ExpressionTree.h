#ifndef __EXPRESSION_TREE__
#define __EXPRESSION_TREE__

#include "tokenizer.h"
#include "stack.h"

/* The parser (non-existent unit) will take a stream of tokens and build
 * structure (ExpresssionTree) based on the "grammatical" rules of computer
 * arithmetic.
 */

// Inspired by https://github.com/PixelRifts/math-expr-evaluator/tree/master
enum node_type_t {
	NODE_LITERAL,	
	NODE_VARIABLE,  
	NODE_LPAREN,
	NODE_RPAREN,
	NODE_BINARY_ADD,
	NODE_BINARY_MINUS,
	NODE_BINARY_MULT,
	NODE_BINARY_DIV,
	NODE_BINARY_MOD,
	NODE_UNARY_INC,
	NODE_UNARY_DEC,
	NODE_UNARY_MINUS,
	NODE_UNARY_PLUS,
	NODE_ERROR
};

typedef struct Tnode Tnode;
typedef Tnode *ExpressionTree;	// an expression tree is the memory address of a Tnode

// Tnode: a node in the parse tree (AST)
// 	- token: Token (have token string and length)
// - can either be a leaf node, unary operator node, or a binary operator node
struct Tnode {
	enum node_type_t type;
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
