#ifndef __EXPRESSION_TREE__
#define __EXPRESSION_TREE__

#include "tokenizer.h"
#include "stack.h"

// Inspired by https://github.com/PixelRifts/math-expr-evaluator/tree/master

// Tnode: a node in the parse tree (AST)
// 	- token: Token (have token string and length)
// - can either be a leaf node, unary operator node, or a binary operator node
typedef struct ASTNode {
	Token token;
	long value;	// the (partial) result of the entire expression evaluated at this node 
			// value == NAN indicates this node is a TOK_VAR
	union {
		struct { struct ASTNode *operand; } unary;
		struct { struct ASTNode *left; struct ASTNode *right; } binary;
	};
} ASTNode;

typedef ASTNode *ExpressionTree;

#define NAN 0xffUL << 23 | 1	// a (bit) pattern resembling a not-a-number 32-bit float by IEEE-754
#define panic(msg) {fprintf(stderr, "%s at line %d\n", msg, __LINE__); assert(false);}

ExpressionTree expressiontree_build_tree(Tokenizer *tkz);
void expressiontree_print_to_file(FILE *fp, ExpressionTree root);
void expressiontree_destroy_tree(ExpressionTree *root);

#endif /* end of __EXPRESSION_TREE__ */
