#include "../headers/tokenizer.h"

static bool _is_valid_operator(char ch);
static int _raw_length(char const *raw);

Tokenizer init_tokenizer(char const *raw)
{
  return (Tokenizer){
      .raw = raw,
      .length = _raw_length(raw),
      .tokens = NULL,
      .n_tokens = 0};
}

void tokenize(Tokenizer *a_tkz)
{ // fill in tokens and n_tokens fields
  assert(a_tkz);

  // assume len(tokens) <= len(raw)
  a_tkz->tokens = malloc(sizeof(*(a_tkz->tokens)) * a_tkz->length);

  // count number of tokens in raw and update n_tokens
  for (char *curr = a_tkz->raw; *curr != ';'; curr += 1)
  {
    assert(a_tkz->n_tokens < a_tkz->length);
    if (_is_valid_operator(*curr) || isalnum(*curr))
    {
      a_tkz->tokens[a_tkz->n_tokens] = *curr;
      a_tkz->n_tokens += 1;
    }
  }
  a_tkz->tokens[a_tkz->n_tokens + 1] = '\0';
}

void destroy_tokenizer(Tokenizer *a_tkz)
{
  assert(a_tkz);
  if (a_tkz->raw)
  {
    free((void *)a_tkz->raw);
  }
  if (a_tkz->tokens)
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

static int _raw_length(char const *raw)
{
  // this is not counting the null byte '\0'
  if (raw)
  {
    int len = 0;
    while (raw[len] != '\0')
    {
      len += 1;
    }
    return len;
  }
  return -1;
}