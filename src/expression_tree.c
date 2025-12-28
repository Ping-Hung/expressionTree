#include "../headers/ExpressionTree.h"
#include "../headers/Parser.h"

//  Unary('++'|'--') > Unary('+'|'-') > binary('*'|'/'|'%') > binary('+' | '-')
typedef unsigned char precedence_t;

// static helpers
static inline int _expr_error_idx(Token *expr, size_t length);
static inline ExpressionTree _parse_expr(Parser *parser, precedence_t min_bp);

// Regardless of what expr actually looks like, expressiontree_build_tree will
// assume the rules/conventions of infix mathematical expressions and build a
// tree according grammar that defines all possible math expressions
ExpressionTree expressiontree_build_tree(Tokenizer *tkz)
{
	assert(tkz && "parameter tkz must be a valid Tokenizer *");
	// handle lexing errors
	int error = _expr_error_idx(tkz->tokens, tkz->n_tokens);
	if (error != tkz->n_tokens) {
		char const *expr = tkz->tokens[0].token_string;
		int expr_len = tkz->tokens[error].token_string + tkz->tokens[error].length - expr;
		if (error > -1) {
			fprintf(stderr, "expression \"%.*s\" contains invalid token \"%.*s\"\n",
					expr_len, expr,
					(int)tkz->tokens[error].length, tkz->tokens[error].token_string);
		} else {
			fprintf(stderr, "expression \"%.*s\" has unmatched number of \"(\" and \")\"\n"
					,expr_len, expr);
		}
		return NULL;
	}

	Parser parser = parser_init(tkz);
	ExpressionTree root = _parse_expr(&parser, 0);
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

static inline int _expr_error_idx(Token *expr, size_t length)
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
	return (n_lparen == n_rparen) ? length : -1;
}

static inline ExpressionTree _parse_expr(Parser *parser, precedence_t min_bp)
{
	// Goal: use Pratt parsing to parse infix expressions
	assert(parser && "parameter parser must be a valid Parser *");

	return NULL;
}
