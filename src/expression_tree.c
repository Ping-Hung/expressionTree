#include "../headers/ExpressionTree.h"
#include "../headers/stack.h"
#include "../headers/Parser.h"

typedef char precedence_t;
typedef struct {precedence_t lbp, rbp;} binding_power_t;

// common data structures
static char const *operatorSymbolLUT[] = {
	[TOK_ADD] 	= "+",
	[TOK_MINUS]	= "-",
	[TOK_MULT]	= "*",
	[TOK_DIV]	= "/",
	[TOK_MOD]	= "%",
	[TOK_INC]	= "++",
	[TOK_DEC]	= "--"
};

binding_power_t binaryOperatorLUT[] = {
        // highest precedence is at the bottom
	[TOK_ADD] 	= {.lbp = 1, .rbp = 2},
	[TOK_MINUS]	= {.lbp = 1, .rbp = 2},

	[TOK_MULT]	= {.lbp = 3, .rbp = 4},
        // implicit multiplication
        [TOK_LPAREN]    = {.lbp = 3, .rbp = 4},
        [TOK_LIT]       = {.lbp = 3, .rbp = 4},
        [TOK_VAR]       = {.lbp = 3, .rbp = 4},

	[TOK_DIV]	= {.lbp = 3, .rbp = 4},
	[TOK_MOD]	= {.lbp = 3, .rbp = 4}
};

binding_power_t unaryOperatorLUT[] = {
        // highest precedence is at the bottom
        [TOK_LPAREN]    = {.lbp = 0, .rbp = 0},
        [TOK_LIT]       = {.lbp = 0, .rbp = 0},
        [TOK_VAR]       = {.lbp = 0, .rbp = 0},

	[TOK_ADD] 	= {.lbp = 0, .rbp = 6},
	[TOK_MINUS]	= {.lbp = 0, .rbp = 6},

        [TOK_INC]       = {.lbp = 0, .rbp = 8},
        [TOK_DEC]       = {.lbp = 0, .rbp = 8}
};


// static helpers declaration
// common helper
static inline ExpressionTree _alloc_node();
// lexical error handling
static inline int _expr_error_idx(Token *expr, size_t length);

// binding power assignment
static inline binding_power_t _assign_prefix(Token token);
static inline binding_power_t _assign_infix(Token token);

// expression parsing
static inline ExpressionTree _parse_expr(Parser *parser);
static inline ExpressionTree _parse_atom(Parser *parser, precedence_t curr_bp);
static inline ExpressionTree _parse_prefix(Parser *parser, precedence_t curr_bp);
static inline ExpressionTree _parse_infix(Parser *parser, ExpressionTree lhs, precedence_t curr_bp);
static inline ExpressionTree _parse_postfix(Parser *parser, ExpressionTree lhs);

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
		fprintf(stderr, "no parse tree was built.\n");
		return NULL;
	}
	// actual parsing
	Parser parser = parser_init(tkz);
	ExpressionTree root = _parse_expr(&parser);
	return root;
}

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

static inline ExpressionTree _alloc_node()
{
        ExpressionTree node = malloc(sizeof(*node));
        if (!node) {
                panic("malloc failed when allocating ASTNode");
        }
        *node = (ASTNode) { 0 };        // zero out allocated struct to prevent uninit. memory access
        return node;
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

static inline binding_power_t _assign_prefix(Token token)
{
	switch (token.type) {
        case TOK_EOF: case TOK_RPAREN: case TOK_ERROR:
        case TOK_MULT: case TOK_DIV: case TOK_MOD: 
		panic("bad token passed to _assign_prefix");
        default:
                return unaryOperatorLUT[token.type];
	}
}

static inline binding_power_t _assign_infix(Token token)
{
        switch (token.type) {
        case TOK_EOF: case TOK_RPAREN: case TOK_ERROR:
        case TOK_INC: case TOK_DEC:
		panic("bad token passed to _assign_infix");
        default:
                return binaryOperatorLUT[token.type];
        }
}

static inline ExpressionTree _parse_expr(Parser *parser)
{
	/*      Entry point of all forms expression parsing, assume infix by default.
	 *  			expr := expr op expr
	 *  1) Build up the leftmost expr to the right of := and store it in lhs
         *          - look at the first token parser points/refers to and deduce its semantic meaning
         *  2) if there are more tokens after the first expr, call _parse_infix, and store the result
         *     in lhs.
         *  3) return lhs when no tokens are left
	 */ 
	assert(parser && "parameter parser must be a valid Parser *");

	// make lhs
	ExpressionTree lhs = NULL;
        Token tok = parser_peek(parser);
	switch (tok.type) {
        case TOK_EOF:	
        // base case: empty expr ⇒  empty node (Ø)
                return NULL;
        case TOK_VAR: case TOK_LIT: case TOK_LPAREN:
                lhs = _parse_atom(parser, 0);
                break;
        case TOK_ADD: case TOK_MINUS: case TOK_INC: case TOK_DEC:
        // prefix expressions, call with 0 so _parse_prefix can handle the rest independently
                lhs = _parse_prefix(parser, 0);
                break;
        default:
                panic("bad token at _parse_expr");
	}

        // lhs should be completely parsed at this stage
	// parser->curr[0] should now be an operator
        while (1) {
                tok = parser_peek(parser);
                switch (tok.type) {
                case TOK_EOF: case TOK_ERROR:
                        goto exit;
                case TOK_RPAREN:
                // ')' marks the end of a nested expression, skip them and return to the caller
                        parser_advance(parser);
                        goto exit;
                default:
                        break;
                }
                // now tok **must** be an operator, so use _parse_infix to handle it
                lhs = _parse_infix(parser, lhs, 0);     // passing 0 in each time might be the problem
	}
exit:
	return lhs;
}

static inline ExpressionTree _parse_atom(Parser *parser, precedence_t curr_bp)
{
	/*
	 *   Atom := TOK_LIT | TOK_VAR | '(' expr ')'
	 *     -  makes call to _parse_expr, which could call _parse_atom (indirectly recursive)
	 */
	ExpressionTree node = NULL;
        Token tok = parser_peek(parser);
	switch (tok.type) {
	// base cases: EOF | TOK_VAR | TOK_LIT
	case TOK_EOF:
		break;
	case TOK_VAR: case TOK_LIT:
		node = _alloc_node();
		node->token = tok;
		node->value = (node->token.type == TOK_LIT) ? atol(node->token.token_string) : NAN;
                // technically don't need following inits, but do it to show we are making leaf nodes
		node->binary.left  = NULL; 
		node->binary.right = NULL; 
                // single TOK_LIT|TOK_VAR is potentially a postfix expression, calling _parse_postfix
                node = _parse_postfix(parser, node);  
		break;
	case TOK_LPAREN:
        // (indirectly) recursive case: 
		parser_advance(parser);
		node = _parse_expr(parser);
		break;
	default:
		panic("expecting TOK_VAR|TOK_LIT|'(' as the first token in _parse_atom");
	}
        // NEVER advance the parser past the ')' in this function, this extra advancement disturbs
        // the grouping of parenthesized expressions. Leave the task to the callers (_parse_expr)
	return node;
}

static inline ExpressionTree _parse_prefix(Parser *parser, precedence_t curr_bp)
{
        /* Dedicated function to parse prefix expressions 
         *           prefix := op prefix |  Atom |  EOF
         */
        assert(parser && "parameter parser needs to be a valid Parser *");

        ExpressionTree node = NULL;
        binding_power_t bp = (binding_power_t) { 0 };

	switch (parser_peek(parser).type) {
        // base cases
        case TOK_EOF:
                break;
        case TOK_LIT: case TOK_VAR: case TOK_LPAREN:
                node = _parse_atom(parser, 0);
                break;
        // recursive cases
	case TOK_ADD: case TOK_MINUS: case TOK_INC: case TOK_DEC:
		node = _alloc_node();
		node->token = parser_peek(parser);
		node->value = 0;

		parser_advance(parser);
                bp = _assign_prefix(parser_peek(parser));
                if (curr_bp <= bp.rbp) { 
                // compare how "tight" the new token(s) binds to the right, ≤ curr_bp means they
                // reside lower in the tree. Recursively build them.
                        node->unary.operand = _parse_prefix(parser, bp.lbp);
                }
		break;
	default:
		panic("bad token in _parse_prefix");
	}

	return node;
}

static inline ExpressionTree _parse_infix(Parser *parser, ExpressionTree lhs, precedence_t curr_bp)
{
        /*              Function that parses infix expression                   
         *                       expr := expr op expr
         * - Handle ``op expr`` part of an infix expression
         * - Expects parser to point to an operator or a ')' token before loop.
         *   **note: a single TOK_LIT and TOK_VAR is equivalent to a '*' (implicit multiplication)**
         * - Shall assume lhs is a valid ASTNode
         */
	assert(parser && "parameter parser must be a valid Parser *");
        assert(lhs && "parameter lhs must be a valid ExpressionTree (ASTNode *)");
        binding_power_t bp = (binding_power_t) { 0 };
        while (1) {
                Token tok = parser_peek(parser);
                switch (tok.type) {
                case TOK_ERROR: case TOK_EOF:
                        goto exit;
                case TOK_RPAREN:
                        parser_advance(parser);
                        continue;
                default:
                        break;
                }

                // decide how "tight" op binds to the left and right
                bp = (tok.type == TOK_INC || tok.type == TOK_DEC) ? 
                        _assign_prefix(tok) :
                        _assign_infix(tok);

                if (curr_bp > bp.lbp) {
                        break;
                }

                // make an op node
                ExpressionTree op = _alloc_node();
                op->token = tok;
                op->value = 0;
                op->binary.left = lhs;

                switch (tok.type) {
                case TOK_LIT: case TOK_VAR: case TOK_LPAREN:
                        op->token = (Token) {.type = TOK_MULT, .token_string = "*", .length = 1};
                        op->binary.right = _parse_atom(parser, bp.rbp);
                        lhs = op;
                        continue;
                case TOK_INC: case TOK_DEC:
               // (lhs op) is a postfix expression
                        lhs = _parse_postfix(parser, op);
                        continue;
                default:
                        break;
                } 

                // parse the last expr 
                parser_advance(parser);
                lhs = op;
                lhs->binary.right = _parse_expr(parser);
        }
exit:
        return lhs;
}

static inline ExpressionTree _parse_postfix(Parser *parser, ExpressionTree lhs)
{
        /*           Funtion that parses general postfix expressions
         *                      postfix := Atom+ ['++'|'--']*
         * It should:
         *      - Construct an AST for postfix expressions.
         *      - Change where lhs is referring to (pointing to) when neccessary.
         *      - Advance parser to consume all the {'++', '--'} tokens.
         *      - Move parser->curr  to the start of the expression following this postfix expression
         *        before returning.
         */
        assert(parser && "parameter parser needs to be a valid Parser *");
        assert(lhs && "parameter lhs needs to be a valid ExpressionTree");
        if (!(lhs->token.type == TOK_LIT || lhs->token.type == TOK_VAR ||
              lhs->token.type == TOK_INC || lhs->token.type == TOK_DEC)) {
                panic("paramenter lhs must be either a LIT, VAR, '++' or '--' in parse_postfix");
        }

        while (1) {
                parser_advance(parser);
                Token tok = parser_peek(parser);
                if (!(tok.type == TOK_INC || tok.type == TOK_DEC)) {
                        break;
                }
                ExpressionTree top_op = _alloc_node();
                // somehow compound literal initialization will result in ASTNode loss (loosing lhs)
                top_op->token = tok;
                top_op->value = 0;
                top_op->unary.operand = lhs;
                lhs = top_op;
        }
        return lhs;
}
