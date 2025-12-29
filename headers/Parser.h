#ifndef __PARSER_H__
#define __PARSER_H__

#include "tokenizer.h"

typedef struct {
	Token *curr;
	Token const *end;
} Parser;  // 0 ≤ end - curr < n_tokens

static int n_tokens;

static inline Parser parser_init(Tokenizer *tkz)
{
	assert(tkz && "parameter tkz must be a valid Tokenizer *");
	n_tokens = tkz->n_tokens;
	return (Parser) {.curr = tkz->tokens, .end = tkz->tokens + tkz->n_tokens};
}

static inline bool parser_parse_completed(Parser *parser)
{
	return parser->curr == parser->end || parser->curr->type == TOK_EOF;
}

static inline Token parser_peek(Parser *parser)
{
	// returns the token parser->curr is pointing to
	assert(parser && "parameter tkz must be a valid Parser *");
	assert(parser->curr < parser->end);

	return parser->curr[0];
}

static inline Token parser_advance(Parser *parser)
{
	// advance curr, then return the token it was pointing to before hand
	assert(parser && "parameter tkz must be a valid Parser *");
	assert(parser->curr < parser->end);

	parser->curr++;
	return parser->curr[-1];
}

#endif /* end of __PARSER_H__ */
