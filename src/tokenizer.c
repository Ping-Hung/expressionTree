#include <stdio.h>
#include "../headers/tokenizer.h"

static int _raw_length(char const *raw);
static void _update_tokens_and_n_tokens(Tokenizer *a_tkz);
static void _array_fillin(Tokenizer *a_tkz);
static void _init_validTokens();

Tokenizer init_tokenizer(char const *raw)
{
  _init_validTokens();
  return (Tokenizer){
      .raw = raw,
      .length = _raw_length(raw),
      .tokens = NULL,
      .n_tokens = 0};
}

void tokenize(Tokenizer *a_tkz)
{ // fill in tokens, n_tokens, and array fields
  assert(a_tkz);

  // assume len(tokens) <= len(raw)
  a_tkz->tokens = malloc(sizeof(*(a_tkz->tokens)) * a_tkz->length);
  // count number of tokens in raw and update n_tokens
  _update_tokens_and_n_tokens(a_tkz);

  // fill in array
  a_tkz->array = malloc(sizeof(*(a_tkz->array)) * a_tkz->n_tokens);
  _array_fillin(a_tkz);
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
  if (a_tkz->array)
  {
    free(a_tkz->array);
  }
}

static void _init_validTokens()
{
  char operators[] = {'+', '-', '*', '/', '%', '(', ')'};
  for (int i = 0; i < sizeof(operators) / sizeof(operators[0]); ++i)
  {
    int index = operators[i];
    validTokens[index] = operators[i];
  }

  // alphabets
  for (char ch = 'a'; ch <= 'z'; ++ch)
  {
    int idx = ch;
    validTokens[idx] = ch;
  }

  for (char ch = 'A'; ch <= 'Z'; ++ch)
  {
    int idx = ch;
    validTokens[idx] = ch;
  }

  // numerics
  for (char ch = '0'; ch <= '9'; ++ch)
  {
    int idx = ch;
    validTokens[idx] = ch;
  }
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

static void _update_tokens_and_n_tokens(Tokenizer *a_tkz)
{
  for (char const *a_ch = a_tkz->raw; *a_ch != ';'; a_ch += 1)
  {
    assert(a_tkz->n_tokens < a_tkz->length);
    int index = *a_ch;
    if (validTokens[index] != '\0')
    {
      a_tkz->tokens[a_tkz->n_tokens] = *a_ch;
      a_tkz->n_tokens += 1;
    }
  }
  a_tkz->tokens[a_tkz->n_tokens + 1] = '\0';
}

static void _array_fillin(Tokenizer *a_tkz)
{
  if (!a_tkz->array)
  {
    fprintf(stderr, "a_tkz->array is not alloc'd\n");
    assert(a_tkz->array);
  }
}