#include "../headers/ExpressionTree.h"
#include "../headers/stack.h"
#include "../headers/Parser.h"

typedef char precedence_t;
// static helpers declaration
// lexical error handling
static inline int _expr_error_idx(Token *expr, size_t length);
// binding power assignment
static inline void _prefix_bp(precedence_t *lbp, precedence_t *rbp, Token token);
static inline void _infix_bp(precedence_t *lbp, precedence_t *rbp, Token token);
// expression parsing
static inline ExpressionTree _parse_atom(Parser *parser, precedence_t curr_bp);
static inline ExpressionTree _parse_prefix(Parser *parser, precedence_t curr_bp);
static inline ExpressionTree _parse_expr(Parser *parser, precedence_t curr_bp);

// main apis
ExpressionTree expressiontree_build_tree(Tokenizer *tkz)
{
	assert(tkz && "parameter tkz must be a valid Tokenizer *");
	// handle lexing errors
	int error = _expr_error_idx(tkz->tokens, tkz->n_tokens);
	if (error != tkz->n_tokens) {
		char const *expr = tkz->tokens[0].token_string;
		int expr_len = tkz->tokens[tkz->n_tokens - 1].token_string +
			       tkz->tokens[tkz->n_tokens - 1].length - expr;

		if (error > -1) {
			fprintf(stderr, "expression \"%.*s\" contains invalid token \"%.*s\"\n",
					expr_len, expr,
					(int)tkz->tokens[error].length, tkz->tokens[error].token_string);
		} else {
			fprintf(stderr, "expression \"%.*s\" has invalid pairs of parentheses\n",
					expr_len, expr);
		}
		return NULL;
	}
	// actual parsing
	Parser parser = parser_init(tkz);
	ExpressionTree root = _parse_expr(&parser, 0);
	return root;
}

char const *operatorSymbolLUT[] = {
	[TOK_ADD] 	= "+",
	[TOK_MINUS]	= "-",
	[TOK_MULT]	= "*",
	[TOK_DIV]	= "/",
	[TOK_MOD]	= "%",
	[TOK_INC]	= "++",
	[TOK_DEC]	= "--"
};

void expressiontree_print_to_file(FILE *fp, int depth, ExpressionTree root)
{
	assert(fp);
	if (root) {
		// %*s (pad * of spaces in front of string s), %.*s (print at most * characters of string)
		if (depth > 0) {
			fprintf(fp, "%*s%*s",
				depth, " ",
				depth, "|__");
		}

		if (root->token.type == TOK_VAR || root->token.type == TOK_LIT ) {
			fprintf(fp, "\"%.*s\"\n", (int)root->token.length, root->token.token_string);
		} else {
			fprintf(fp, "\"%s\"\n", operatorSymbolLUT[root->token.type]);
		}

		expressiontree_print_to_file(fp, depth + 1, root->binary.left);
		expressiontree_print_to_file(fp, depth + 1, root->binary.right);
	}
}

void expressiontree_destroy_tree(ExpressionTree *root)
{
	// freeing tree via a post order tree walk
	assert(root && "arg root must be a valid ExpressionTree * (ASTNode **)");
	if (*root) {
		// recursively free the tree (treating ExpressionTree as regular Binary Tree)
		expressiontree_destroy_tree(&(*root)->binary.left);
		expressiontree_destroy_tree(&(*root)->binary.right);
		free(*root);
		*root = NULL;
	}
}

static inline int _expr_error_idx(Token *expr, size_t length)
{
	// look for error tokens and track the number of '(' and ')'
	if (expr[0].type == TOK_RPAREN) {
		// no expression should start with a ')'
		return -1;
	}

	StackFrame *stack = NULL;
	for (Token *tok = expr; tok < expr + length; tok++) {
		switch (tok->type) {
		case TOK_ERROR:
			return tok - expr;
		case TOK_LPAREN:
			push(&stack, tok);
			break;
		case TOK_RPAREN:
			pop(&stack);
			break;
		default:
			break;
		}
	}

	bool good_paren = is_empty(stack);
	while (!is_empty(stack)) {
		pop(&stack);
	}

	return good_paren ? length : -1;
}

static inline void _prefix_bp(precedence_t *lbp, precedence_t *rbp, Token token)
{
	assert("parameter lbp and rbp must be valid precedence_t *'s" && lbp && rbp);
	switch (token.type) {
	case TOK_ADD: case TOK_MINUS:
		{*lbp = 0, *rbp = 2;}
		break;
	case TOK_INC: case TOK_DEC:
		{*lbp = 0, *rbp = 4;}
		break;
	default:
		panic("bad prefix token in _prefix_bp");
	}
}

static inline void _infix_bp(precedence_t *lbp, precedence_t *rbp, Token token)
{
	assert("parameter lbp and rbp must be valid precedence_t *'s" && lbp && rbp);
	switch (token.type) {
	case TOK_ADD: case TOK_MINUS:
		// need to consider if they are stand alone prefix operators
		{*lbp = 1, *rbp = 2;}
		break;
	case TOK_MULT: case TOK_DIV: case TOK_MOD:
	case TOK_LIT: case TOK_VAR:
		{*lbp = 3, *rbp = 4;}
		break;
	default:
		panic("bad infix token type passed to _infix_bp");
	}
}

static inline ExpressionTree _parse_atom(Parser *parser, precedence_t curr_bp)
{
	// Atom := TOK_LIT | TOK_VAR | '(' expr ')'
	// indirectly recursive
	ExpressionTree node = NULL;
	switch (parser_peek(parser).type) {
	// base cases
	case TOK_EOF:
		return node;
	case TOK_VAR: case TOK_LIT:
		if (!(node = malloc(sizeof(*node)))) {
			panic("malloc failed in _parse_atom");
		}
		node->token = parser_peek(parser);
		node->value = (node->token.type == TOK_LIT) ? atol(node->token.token_string) : NAN;
		node->binary.left = NULL;
		node->binary.right= NULL;

		parser_advance(parser);
		return node;
	case TOK_LPAREN:
		// recursive case: 
		parser_advance(parser);
		node = _parse_expr(parser, 0);
		parser_advance(parser);		// move parser->curr past ')'
		return node;
	default:
		panic("bad token type in _parse_atom");
	}
}

static inline ExpressionTree _parse_prefix(Parser *parser, precedence_t curr_bp)
{
	// indirectly recursive (calling _parse_expr)
	ExpressionTree node = NULL;
	switch (parser_peek(parser).type) {
	case TOK_EOF:
		// base case: empty expression
		return node;

	case TOK_LIT: case TOK_VAR: case TOK_LPAREN:
		// potentially recursive (TOK_LPAREN)
		return _parse_atom(parser, curr_bp);

	case TOK_ADD: case TOK_MINUS: case TOK_INC: case TOK_DEC: 
		// node is an operator, can follow ('+'|'-'|'++'|'--')* Atom
		node = malloc(sizeof(*node));
		if (!node) {
			panic("malloc failed in _parse_prefix");
		}
		node->token = parser_peek(parser);
		node->value = 0;
		node->binary.right = NULL;

		// recursively parse the rhs
		precedence_t lbp, rbp;
		_prefix_bp(&lbp, &rbp, parser_peek(parser));
		parser_advance(parser);
		node->unary.operand = _parse_prefix(parser, rbp);

		return node;
	default:
		panic("bad token in _parse_prefix");
	}
}


static inline ExpressionTree _parse_expr(Parser *parser, precedence_t curr_bp)
{

	/*  this routine will be handling general expressions, assume infix by default.
	 *  Roughly breaking expression (expr) down to 3 sections, 
	 *  		 expr := lhs op rhs
	 *  Use (indirect + direct) recursion to parse this entire thing
	 */ 
	// A variant of Pratt Parsing
	// every token has a meaning, this meant computation precedence in this context,
	// which affect their relative position in the resulting ExpressionTree
	assert(parser && "parameter parser must be a valid Parser *");

	// make lhs
	ExpressionTree lhs = NULL;
	precedence_t lbp, rbp;

	switch (parser_peek(parser).type) {
	case TOK_EOF:	
	// base case: if given expression is empty, simply return an empty node
		return NULL;
	// Atom := TOK_LIT | TOK_VAR | '(' expr ')'
	case TOK_VAR: case TOK_LIT: case TOK_LPAREN:
		lhs = _parse_atom(parser, 0);
		break;
	case TOK_ADD: case TOK_MINUS: case TOK_INC: case TOK_DEC:
		// prefix expression
		_prefix_bp(&lbp, &rbp, parser_peek(parser));
		lhs = _parse_prefix(parser, rbp);
		break;
	default:
		panic("bad token at _parse_expr before loop");
	}

	// I shall assume parser->curr is the last token of lhs at this point
	if (parser_peek(parser).type == TOK_RPAREN) {
		parser_advance(parser);
	}
	assert("no ')' before loop" && parser_peek(parser).type != TOK_RPAREN);

	// parse the rest of the expression (lhs is completely parsed)
	// parser->curr should point to an operator 
	for (Token tok = parser_peek(parser); tok.type != TOK_ERROR && tok.type != TOK_EOF;
	     tok = parser_peek(parser)) {

		// build op (an ASTNode holding an operator)
		ExpressionTree op = malloc(sizeof(*op));
		if (!op) {
			panic("malloc failed in _parse_expr");
		}

		*op = (ASTNode) {
			.token = tok,
			.value = 0,
			.binary.left = lhs,
			.binary.right = NULL
		};

		if (op->token.type == TOK_INC || op->token.type == TOK_DEC) {
			// (lhs op) is a postfix expression
			lhs = op;
			parser_advance(parser);
			continue;
		}

		// get binding power of the operator
		_infix_bp(&lbp, &rbp, op->token);

		if (curr_bp > lbp) {
			free(op);
			break;
		}

		// use recursion to build rhs, 
		// make sure parser->curr is at the beginning of rhs expression
		parser_advance(parser);
		op->binary.right = _parse_expr(parser, rbp);
		lhs = op;
	}
	return lhs;
}
