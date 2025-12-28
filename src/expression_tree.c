#include "../headers/ExpressionTree.h"
#include "../headers/stack.h"
#include "../headers/Parser.h"

//  Unary('++'|'--') > Unary('+'|'-') > binary('*'|'/'|'%') > binary('+' | '-')
typedef char precedence_t;

// static helpers
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

	bool valid_parentheses = is_empty(stack);
	while (!is_empty(stack)) {
			pop(&stack);
	}
	return valid_parentheses ? length : -1;
}

static inline ExpressionTree _make_node(Token tok)
{
	// Recall: typedef ASTNode *ExpressionTree
	ExpressionTree node = malloc(sizeof(ASTNode));
	if (!node) {
		panic("malloc failed while allocating ASTNode");
	}

	switch (tok.type) {
	case TOK_LIT: 
		*node = (ASTNode) {
			.type = tok.type,
			.value = atol(tok.token_string),
			.unary.operand = NULL
		};
		break;
	case TOK_VAR:
		*node = (ASTNode) {
			.type = tok.type,
			.value = NAN,
			.unary.operand = NULL
		};
		break;
	// binary operators
	case TOK_ADD: case TOK_MINUS:
	case TOK_MULT: case TOK_DIV: case TOK_MOD:
		*node = (ASTNode) {
			.type = tok.type,
			.value = 0,
			.binary.left = NULL,
			.binary.right = NULL
		};
		break;
	// unary operators
	case TOK_INC: case TOK_DEC:
		*node = (ASTNode) {
			.type = tok.type,
			.value = 0,
			.unary.operand = NULL
		};
		break;
	default: 
		free(node);
		panic("bad token type");
	}
	return node;
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
		panic("bad token type");
	}
}

static inline ExpressionTree _pratt_parse(Parser *parser, precedence_t curr_bp)
{
	// Goal: use Pratt parsing to parse infix expressions 
	assert(parser && "parameter parser must be a valid Parser *");
	// consume the very first token and make a node 
	ExpressionTree lhs = NULL;
	Token tok = parser_advance(parser);
	if (tok.type == TOK_LPAREN) {
		// a '(' indicates existence of a group expression (which has higher binding power). 
		// Build it up recursively before the rest of the expression.
		lhs = _pratt_parse(parser, curr_bp + 1); 
		assert("expected to see a ')' after parsing a \"'('expr\"" &&
			parser_advance(parser).type == TOK_RPAREN);
	} else {
		lhs = _make_node(tok);
	}

	// parse the rest of the expression
	// parser->curr should point to an operator at the beginning of each loop iteration
	ExpressionTree op = NULL;
	while (true) {
		tok = parser_advance(parser);
		op = _make_node(tok);
		// get operator binding power 
		// depending on the type of lhs, decide what type of operator op is
		precedence_t lbp, rbp;
		switch (lhs->type) {
		case TOK_ADD: case TOK_MINUS:
			// prefix operator
			_prefix_bp(&lbp, &rbp);
			break;
		case TOK_INC: case TOK_DEC:
			// prefix or postfix, lookahead once to see
		default:
			// infix operator
			_infix_bp(&lbp, &rbp);
			break;
		}

		if (curr_bp > lbp) {
			// if the operator we look at have lower precedence than current 
			// binding power, stop
			break;
		}
	}

	return op;
}



// main apis
ExpressionTree expressiontree_build_tree(Tokenizer *tkz)
{
	/* Regardless of what expr actually looks like, expressiontree_build_tree will
	 * assume the rules/conventions of infix mathematical expressions and build a
	 * tree according grammar that defines all possible math expressions
	 * */
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

	Parser parser = parser_init(tkz);
	ExpressionTree root = _pratt_parse(&parser, 0);
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


