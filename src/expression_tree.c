#include "../headers/ExpressionTree.h"

typedef char precedence_t
//  Unary('++'|'--') > Unary('+'|'-') > binary('*'|'/'|'%') > binary('+' | '-')
static precedence_t preced_LUT[] = {
	// only the precedence of legal operators
	[NODE_ERROR]        = -1, // this is a "trick" in the hopes to ease programming
	[NODE_BINARY_ADD]   = 0,
	[NODE_BINARY_MINUS] = 0,
	[NODE_BINARY_MULT]  = 1
	[NODE_BINARY_DIV]   = 1,
	[NODE_BINARY_MOD]   = 1,
	[NODE_UNARY_PLUS]   = 2,
	[NODE_UNARY_MINUS]  = 2,
	[NODE_UNARY_INC]    = 3,
	[NODE_UNARY_DEC]    = 3
};

static precedence_t _get_operator_precedence(Token curr)
{
	precedence_t p_type = NODE_ERROR;
	if (curr.type == TYPE_OP) {
		if (curr.length == 2) {
			// unary increment or decrement, have to decide prefix or postfix
		} else {
			// binary operations
			switch (curr.token_string[0]) {
			case '+': p_type = NODE_BINARY_ADD;   break;  
			case '-': p_type = NODE_BINARY_MINUS; break;
			case '*': p_type = NODE_BINARY_MULT;  break;
			case '/': p_type = NODE_BINARY_DIV;   break;
			case '%': p_type = NODE_BINARY_MOD;   break;
			default:
				  break;
			}
		}
	}
	return p_type;
}

// Regardless of what expr actually looks like, expressiontree_build_tree will
// assume the rules/conventions of infix mathematical expressions and build a
// tree according to this grammar

/* 		   Grammar:	(I guess writing them out gives me a picture of
 *  		   		 how the parsing process goes and what helpers come into
 *  		   		 play, but error handling is not specified in grammar...)
 *
 */

ExpressionTree expressiontree_build_tree(Token *expr, size_t length)
{
	// loop through token stream expr and build a tree if the expression is valid.

	// consume a TOK_INVALID ⇒  error (found a bad token) ⇒  halt tree build
	// found a TOK_LPAREN ⇒  following expr.precedence += 1
	ExpressionTree root = NULL;
	for (int i = 0; i < length - 1; i++) {
		// TOK_INVALID handling

		// recursive build left

		// find precedence of valid token expr[i]
		precedence_t current_precedence = _get_operator_precedence(expr[i]);
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
	if (!root)	return;
	switch ((*root)->type) {
	case NODE_BINARY_ADD: 
	case NODE_BINARY_MINUS: 
	case NODE_BINARY_MULT: 
	case NODE_BINARY_DIV: 
	case NODE_BINARY_MOD: 
		{
			expressiontree_destroy_tree(&(*root)->binary.left);
			expressiontree_destroy_tree(&(*root)->binary.right);
		}
		break;
	case NODE_UNARY_PLUS:
	case NODE_UNARY_MINUS:
	case NODE_UNARY_INC:
	case NODE_UNARY_DEC:
	case NODE_UNARY_POST_INC:
	case NODE_UNARY_POST_DEC:
		expressiontree_destroy_tree(&(*root)->unary.operand);
		break;
	default:
		break;
	}
	free(*root);
	*root = NULL;
}

