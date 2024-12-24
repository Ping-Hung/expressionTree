#include "tokenizer.h"

static bool _is_valid_operator(char ch);

Tokenizer init_tokenizer(char const *raw)
{
  return (Tokenizer){
      .raw = raw,
      .length = strlen(raw),
      .tokens = NULL,
      .n_tokens = 0
  };
}

void tokenize(Tokenizer *a_tkz)
{ // fill in tokens and n_tokens fields
  assert(a_tkz);
  // assume len(tokens) <= len(raw)
  a_tkz->tokens = malloc(typeof(*(a_tkz->tokens) * a_tkz->length));

  // count number of tokens and update n_tokens
  for(int i = 0; i < a_tkz->length; ++i)
  {
    char ch = a_tkz->raw[i];

    if(isalnum(ch) || _is_valid_operator(ch))
    {
      a_tkz->tokens[i] = ch;
      a_tkz->n_tokens += 1;
    }
  }
}

void destroy_tokenizer(Tokenizer *a_tkz)
{
  assert(a_tkz);
  if(a_tkz->raw) 
  {
    free(a_tkz->raw);
  }
  if(a_tkz->tokens) 
  {
    free(a_tkz->tokens);
  }
}

static bool _is_valid_operator(char ch) 
{
  // now only assume +, -, *, /, (, ) are valid operators
  return (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%' || 
  ch == '(' || ch == ')');
}