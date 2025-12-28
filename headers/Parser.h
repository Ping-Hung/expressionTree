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

static inline Token parser_peek(Parser *parser)
{
	assert(parser && "parameter tkz must be a valid Parser *");
	size_t remaining_tokens = parser->end - parser->curr;
	assert(0 <= remaining_tokens && remaining_tokens < n_tokens &&
			"parser struct invariant violated, see headers/Parser.h");
	assert(parser->curr < parser->end);

	return parser->curr[0];
}

static inline Token parser_advance(Parser *parser)
{
	assert(parser && "parameter tkz must be a valid Parser *");
	size_t remaining_tokens = parser->end - parser->curr;
	assert(0 <= remaining_tokens && remaining_tokens < n_tokens &&
			"parser struct invariant violated, see headers/Parser.h");
	assert(parser->curr < parser->end);
	parser->curr++;
	return parser->curr[-1];
}

#endif /* end of __PARSER_H__ */
