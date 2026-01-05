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

static char const *operatorSymbolLUT[] = {
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
	/*
	 *   Atom := TOK_LIT | TOK_VAR | '(' expr ')'
	 *	  1) case TOK_LIT|TOK_VAR: 
	 *	  	node ← LIT_Node | VAR_Node
	 *	     case '(':
	 *	    	advance parser past '('
	 *	    	node ← _parse_expr(parser, 0)
	 *	    	advance parser past ')'
	 *	  2) advance parser past the token
	 *	  3) return node
	 *    makes call to _parse_expr, which could call _parse_atom (indirectly recursive)
	 */
	ExpressionTree node = NULL;
	switch (parser_peek(parser).type) {
	// base cases: EOF | TOK_VAR | TOK_LIT
	case TOK_EOF:
		break;
	case TOK_VAR: case TOK_LIT:
		node = malloc(sizeof(*node));
		if (!node) {
			panic("malloc failed in _parse_atom");
		}

		node->token = parser_peek(parser);
		node->value = (node->token.type == TOK_LIT) ? atol(node->token.token_string) : NAN;
		node->binary.left = NULL; 
		node->binary.right = NULL; 

		parser_advance(parser);	// move parser->curr past TOK_VAR | TOK_VAR
		break;
	case TOK_LPAREN:
		// recursive case: 
		parser_advance(parser);
		node = _parse_expr(parser, 0);
		break;
	default:
		panic("expecting TOK_VAR|TOK_LIT|'(' as the first token in _parse_atom");
	}

	return node;
}

static inline ExpressionTree _parse_prefix(Parser *parser, precedence_t curr_bp)
{
	ExpressionTree node = NULL;
	precedence_t lbp, rbp;
	// the first token of a prefix expression is one of {'+', '-', '++', '--'}
	switch (parser_peek(parser).type) {
	case TOK_ADD: case TOK_MINUS: case TOK_INC: case TOK_DEC:
		node = malloc(sizeof(*node));
		if (!node) {
			panic("malloc failed in _parse_prefix");
		}
		node->token = parser_peek(parser);
		node->value = 0;
		node->binary.right = NULL; // init this field to prevent memory fault in traversals

		parser_advance(parser);
		_prefix_bp(&lbp, &rbp, node->token);
		// what follows an operator is, in the most general sense, an expression
		node->unary.operand = _parse_expr(parser, rbp);

		break;
	default:
		panic("expecting one of {'+', '-', '++', '--'} as the first token of a prefix expression");
	}

	return node;
}

static inline ExpressionTree _parse_expr(Parser *parser, precedence_t curr_bp)
{
	/*  
	 *  _parse_expr will handle general expressions, assume infix by default.
	 *  This function is the beginning of all kinds of expression parsing.
	 *  			expr := expr op expr
	 *  1) Build up 1st expr, store it in lhs
	 *  	- look at lhs's first token and deduce its semantic meaning
	 *
	 *  2) Advance and see if op expr exists, if so, build 
	 *  			op
	 *  		       /   
	 *  		     lhs
	 *  	and recursively build op->binary.right with _parse_expr
	 *
	 *  3) lhs ← op
	 *  4) return lhs
	 */ 
	assert(parser && "parameter parser must be a valid Parser *");
	precedence_t lbp, rbp;

	// make lhs
	ExpressionTree lhs = NULL;
	switch (parser_peek(parser).type) {
	case TOK_EOF:	
		// base case: empty expr ⇒  empty node (Ø)
		return NULL;
	case TOK_VAR: case TOK_LIT: case TOK_LPAREN:
		// Atom := TOK_LIT | TOK_VAR | '(' expr ')'
		lhs = _parse_atom(parser, 0);
		break;
	case TOK_ADD: case TOK_MINUS: case TOK_INC: case TOK_DEC:
		// prefix expression
		_prefix_bp(&lbp, &rbp, parser_peek(parser));
		lhs = _parse_prefix(parser, rbp);
		break;
	default:
		panic("bad token at _parse_expr");
	}

	if (parser_peek(parser).type == TOK_RPAREN) {
		// indicates that we are looking at the end of a grouped (parenthesized) expression
		// advance parser then return to the caller
		parser_advance(parser);
		return lhs;
	}

	// parser->curr[0] is the last token of lhs at this point (which should not be a ')')
	assert("no ')' before loop" && parser_peek(parser).type != TOK_RPAREN);

	// parse the rest of the expression (lhs is completely parsed)
	// parser->curr should point to an operator token
	while (parser_peek(parser).type != TOK_ERROR && 
	       parser_peek(parser).type != TOK_EOF && 
	       parser_peek(parser).type != TOK_RPAREN) {

		Token tok = parser_peek(parser);
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

		switch (op->token.type) {
		case TOK_INC: case TOK_DEC:
			// (lhs op) is a postfix expression that may follow an operator
			lhs = op;
			parser_advance(parser);
			continue;
		case TOK_LPAREN: case TOK_VAR: case TOK_LIT:
			// implicit multiplication cases:
			// 	lhs '(' expr ')' | lhs TOK_LIT | lhs TOK_VAR
			lhs = _parse_expr(parser, 0);
			op->token = (Token) {
				.type = TOK_MULT, 
				.token_string = "*",
				.length = 1
			};

			op->binary.right = lhs;
			lhs = op;
			continue;
		default:
			break;
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
