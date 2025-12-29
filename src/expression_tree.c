#include "../headers/ExpressionTree.h"
#include "../headers/stack.h"
#include "../headers/Parser.h"

typedef char precedence_t;
// static helpers declaration
static inline int _expr_error_idx(Token *expr, size_t length);
static inline void _prefix_bp(precedence_t *lbp, precedence_t *rbp, Token token);
static inline void _infix_bp(precedence_t *lbp, precedence_t *rbp, Token token);
static inline ExpressionTree _parse_atom(Token tok);
static inline ExpressionTree _parse_prefix(Token tok, Parser *parser, precedence_t curr_bp);
static inline ExpressionTree _parse_postfix(ExpressionTree operator, ExpressionTree lhs);
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

void expressiontree_print_to_file(FILE *fp, ExpressionTree root)
{
	return;
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
		panic("bad token in _prefix_bp");
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
	case TOK_LIT:  case TOK_VAR:	
		// give literals and variable same binding power as
		// multiplicative statements to support implicit multiplication
		{*lbp = 3, *rbp = 4;}
		break;
	default:
		panic("bad token in _infix_bp");
	}
}

static inline ExpressionTree _parse_atom(Token tok)
{
	// no operator or parentheses shall be here
	if (tok.type != TOK_LIT && tok.type != TOK_VAR) {
		panic("expecting TOK_LIT or TOK_VAR in _parse_atom");
		return NULL;
	}

	ExpressionTree node = malloc(sizeof(*node));
	if (!node) {
		panic("malloc failed in _parse_atom");
	}

	*node = (ASTNode) { 
		.token = tok,
		.value = (tok.type == TOK_LIT) ? atol(tok.token_string) : NAN,
		.unary.operand = NULL
	};
	return node;
}

static inline ExpressionTree _parse_postfix(ExpressionTree operator, ExpressionTree lhs)
{
	// operator could be (TOK_LIT, TOK_VAR, TOK_INC, or TOK_DEC)
	// this is not recursive
	ExpressionTree node = NULL;
	switch (operator->token.type) {
	case TOK_INC: case TOK_DEC:
		node = operator;
		node->unary.operand = lhs;
		break;
	case TOK_LIT: case TOK_VAR:
		node = malloc(sizeof(*node));
		if (!node) {
			panic("malloc failed in _parse_postfix");
		}
		node->token = (Token) {.type = TOK_MULT, .token_string = "*", .length = 1};
		node->value = 0;
		node->binary.left  = lhs;
		node->binary.right = operator;
		break;
	default:
		// bad types here...
		break;
	}
	return node;
}

static inline ExpressionTree _parse_prefix(Token tok, Parser *parser, precedence_t curr_bp)
{
	// indirectly recursive (calling _parse_expr)
	ExpressionTree node = NULL;
	switch (tok.type) {
	case TOK_LIT: case TOK_VAR:
		node = _parse_atom(tok);
		break;
	case TOK_ADD: case TOK_MINUS: case TOK_INC: case TOK_DEC: 
		node = malloc(sizeof(*node));
		if (!node) {
			panic("malloc failed in _parse_prefix");
		}
		node->token = tok;
		node->value = 0;
		// the operand of any unary operator is (in its most general sense) an expression
		node->unary.operand = _parse_expr(parser, curr_bp);
		break;
	default:
		fprintf(stderr, "bad token in %s\n", __func__);
	}
	return node;
}


static inline ExpressionTree _parse_expr(Parser *parser, precedence_t curr_bp)
{

	/*  this routine will be handling general expressions, assume infix by default.
	 * _parse_atom, _parse_prefix, and _parse_postfix will be called when meet tokens that 
	 *  possess such semantic meaning.
	 *  Roughly breaking expression (expr) down to 3 sections, 
	 *  		 expr := lhs op rhs
	 *  Use (indirect + direct) recursion to parse this entire thing
	 */ 
	// A variant of Pratt Parsing
	// every token has a meaning, this meant computation precedence in this context,
	// which affect their relative position in the resulting ExpressionTree
	assert(parser && "parameter parser must be a valid Parser *");

	// make lhs
	Token tok = parser_peek(parser);	// *parser->curr == tok
	ExpressionTree lhs = NULL;
	switch (tok.type) {
	case TOK_LPAREN:
		// any expression preceeded by a '(' means nesting
		lhs = _parse_expr(parser, curr_bp + 1); 
		parser_advance(parser);
		tok = parser_advance(parser);
		assert("expected to see a ')' after parsing a \"'('expr\"" && 
			tok.type == TOK_RPAREN);
		break;
	case TOK_VAR: case TOK_LIT:
		lhs = _parse_atom(tok);
		parser_advance(parser);
		break;
	case TOK_ADD: case TOK_MINUS: case TOK_INC: case TOK_DEC:
		// prefix expression
		{
			precedence_t lbp, rbp;
			_prefix_bp(&lbp, &rbp, tok);
			lhs = _parse_prefix(tok, parser, rbp);
			parser_advance(parser);
		}
		break;
	default:
		panic("bad token at _parse_expr");
	}
	assert(parser_peek(parser).type != TOK_RPAREN);
	// no ')' in expr below this exact line

	// parse the rest of the expression (lhs is completely parsed)
	// parser->curr should point to an operator at the beginning of each loop iteration
	ExpressionTree op = lhs;
	ExpressionTree rhs = NULL;
	while (parser_peek(parser).type != TOK_ERROR && parser_peek(parser).type != TOK_EOF) {
		tok = parser_peek(parser);	
		// build op (an ASTNode holding an operator)
		op = malloc(sizeof(*op));
		if (!op) {
			panic("malloc failed in _parse_expr");
		}
		op->token = tok;
		op->value = 0;
		op->binary.left = NULL;

		// get binding power
		precedence_t lbp, rbp;
		if ((lhs->token.type == TOK_VAR || lhs->token.type == TOK_LIT) &&
		     (op->token.type == TOK_INC || op->token.type == TOK_DEC)) {
			// (TOK_VAR|TOK_LIT) followed by ('++'|'--'): (lhs op) is a postfix expression
			lhs = _parse_postfix(op, lhs);
			/* lhs after this point should be
			 *		('++'|'--')
			 *		     |	
			 *		    lhs
			 */ 		    
			parser_advance(parser);
			continue;
		} else if (op->token.type == TOK_VAR || op->token.type == TOK_LIT) {
			// implicit multiplication
			lhs = _parse_postfix(op, lhs);
			/*  lhs after this point should be
			 *		'*'
			 *	       /   \
			 *	     lhs   op
			 */ 		    
			parser_advance(parser);
			continue;
		} else {
			// regular infix operators or error
			op->binary.left = lhs;
			_infix_bp(&lbp, &rbp, op->token);
		}

		if (curr_bp > lbp) {
			break;
		}
		// use recursion to build rhs, if rhs exists
		parser_advance(parser);
		rhs = _parse_expr(parser, rbp);
		
		op->binary.right = rhs;
	}
	return op;
}
