#include "../headers/ExpressionTree.h"
#include "../headers/stack.h"
#include "../headers/Parser.h"

typedef char precedence_t;
typedef struct {precedence_t lbp, rbp;} binding_power_t;

// static helpers 
// common helper
static inline ExpressionTree _alloc_node();

// lexical error handling
static inline int _expr_error_idx(Token *expr, size_t length);

// binding power assignment
static inline binding_power_t _assign_bp(Token token);
static inline binding_power_t _assign_prefix(Token token);
static inline binding_power_t _assign_infix(Token token);

// expression parsing
static inline ExpressionTree _parse_expr(Parser *parser, precedence_t curr_bp);
static inline ExpressionTree _parse_atom(Parser *parser, precedence_t curr_bp);
static inline ExpressionTree _parse_prefix(Parser *parser, precedence_t curr_bp);
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

static inline binding_power_t _assign_bp(Token token)
{
        return (token.type == TOK_INC || token.type == TOK_DEC) ? 
                _assign_prefix(token) :
                _assign_infix(token);
}

static inline binding_power_t _assign_prefix(Token token)
{
	switch (token.type) {
        case TOK_LPAREN: case TOK_LIT: case TOK_VAR:   
        // atoms with the lowest bp
                return (binding_power_t) {.lbp = 0, .rbp = 0};
        case TOK_ADD: case TOK_MINUS:
                return (binding_power_t) {.lbp = 0, .rbp = 6};
        case TOK_INC: case TOK_DEC:
                return (binding_power_t) {.lbp = 0, .rbp = 8};
        default:
		panic("bad token passed to _assign_prefix");
	}
}

static inline binding_power_t _assign_infix(Token token)
{
        switch (token.type) {
        case TOK_ADD: case TOK_MINUS:
                return (binding_power_t) {.lbp = 1, .rbp = 2};
        case TOK_MULT: case TOK_DIV: case TOK_MOD:
        case TOK_LPAREN: case TOK_LIT: case TOK_VAR:
        // implicit multiplication treated like regular multiplication
                return (binding_power_t) {.lbp = 3, .rbp = 4};
        default:
		panic("bad token passed to _assign_infix");
        }
}

static inline ExpressionTree _parse_expr(Parser *parser, precedence_t curr_bp)
{
	/*      Entry point of all forms expression parsing, assume infix by default.
	 *  			expr := expr op expr
	 *  1) Build up the first expr on the rhs of the above rule, and store it in lhs
         *      - parser is advanced and pointing to op after this
         *  2) parse the op node, and recursively parse the last expr
         *  3) return lhs when no tokens are left
	 */ 
	assert(parser && "parameter parser must be a valid Parser *");

	ExpressionTree lhs = _parse_prefix(parser, curr_bp);
        // lhs should be completely parsed at this stage, and parser->curr[0] should now be an
        // operator or a ')'
        while (1) {
                Token tok = parser_peek(parser);
                switch (tok.type) {
                case TOK_EOF: case TOK_ERROR:
                        goto exit;
                case TOK_RPAREN:
                        // ')' means end of a nested expression, return
                        goto exit;
                case TOK_ADD: case TOK_MINUS:
                case TOK_MULT: case TOK_DIV: case TOK_MOD:
                case TOK_LIT: case TOK_VAR: case TOK_LPAREN:
                case TOK_INC: case TOK_DEC:
                        break;
                default:
                        panic("Invalid operator token in _parse_expr");
                }

                // now tok **must** be an operator, decide its relative position in the tree
                binding_power_t bp = _assign_bp(tok);   
                if (curr_bp >= bp.lbp) {
                        // curr_bp ≥ bp.lbp meant current lhs resides lower in the tree, so return
                        break;
                }

                // make an op node
                ExpressionTree op = _alloc_node();
                op->token = tok;
                op->value = 0;
                op->binary.left = lhs;
                switch (tok.type) {
                case TOK_LIT: case TOK_VAR: case TOK_LPAREN:    // implicit multiplication
                        op->token = (Token) {.type = TOK_MULT, .token_string = "*", .length = 1};
                        op->binary.right = _parse_atom(parser, bp.rbp);
                        lhs = op;
                        tok = parser_peek(parser);
                        continue;
                case TOK_INC: case TOK_DEC:     // (lhs op) is a postfix expression
                        lhs = _parse_postfix(parser, op);
                        tok = parser_peek(parser);
                        continue;
                default:        // regular binary operators
                        break;
                } 

                // recursively build expr on the right
                lhs = op;
                parser_advance(parser);
                lhs->binary.right = _parse_expr(parser, bp.rbp);
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
		break;
	case TOK_LPAREN:
        // (indirectly) recursive case: 
		parser_advance(parser);
		node = _parse_expr(parser, 0);
		break;
	default:
		panic("expecting TOK_VAR|TOK_LIT|'(' as the first token in _parse_atom");
	}
        // recall postfix := Atom+['++'|'--']*
        node = _parse_postfix(parser, node);  // parser will be past any postfix expression after this
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
        Token token = parser_peek(parser);
	switch (token.type) {
        // base cases
        case TOK_EOF:
                break;
        case TOK_LIT: case TOK_VAR: case TOK_LPAREN:
                node = _parse_atom(parser, curr_bp);
                break;
        // recursive cases
	case TOK_ADD: case TOK_MINUS: case TOK_INC: case TOK_DEC:
		node = _alloc_node();
		node->token = token;
		node->value = 0;

		parser_advance(parser);
                token = parser_peek(parser);
                bp = _assign_prefix(token);
                if (curr_bp <= bp.rbp) { 
                        // the next token's precedence ≤ current token's precedence:
                        // means it resides lower in the tree, recursively build it.
                        node->unary.operand = _parse_prefix(parser, bp.lbp);
                }
		break;
	default:
		panic("bad token in _parse_prefix");
	}

	return node;
}

static inline ExpressionTree _parse_postfix(Parser *parser, ExpressionTree lhs)
{
        /*           Funtion that parses general postfix expressions
         *                      postfix := Atom+ ['++'|'--']*
         * It should:
         *      - Assumes parser points to ['++'|'--']
         *      - Construct an AST for postfix expressions.
         *      - Change where lhs is referring to (pointing to) when neccessary.
         *      - Advance parser to consume all the {'++', '--'} tokens.
         *      - Move parser->curr to the start of whatever follows this postfix expression
         *        before returning.
         */
        assert(parser && "parameter parser needs to be a valid Parser *");
        assert(lhs && "parameter lhs needs to be a valid ExpressionTree");
        while (1) {
                parser_advance(parser);
                Token tok = parser_peek(parser);
                if (tok.type != TOK_INC && tok.type != TOK_DEC) {
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
