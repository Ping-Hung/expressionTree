#include "../headers/tokenizer.h"

// before using validTokens here, initialize it with the null byte
char validTokens[256] = {'\0'};

static int _raw_length(char const *raw);
static void _update_tokens_and_numchar(Tokenizer *a_tkz);
static void _array_fillin(Tokenizer *a_tkz);
static void _init_validTokens();
static TokenType _get_token_type(char ch);
static inline bool _is_bin_op(char ch);
static void _write_to_buffer(char *dest, char *src, int n_char);

Tokenizer init_tokenizer(char const *raw)
{
  _init_validTokens();
  return (Tokenizer){
      .raw = raw,
      .length = _raw_length(raw),
      .tokens = NULL,
      .numchar = 0,
      .n_tokens = 0};
}

void tokenize(Tokenizer *a_tkz)
{ // fill in tokens, numchar, and array fields
  assert(a_tkz);

  // assume len(tokens) <= len(raw)
  a_tkz->tokens = malloc(sizeof(*(a_tkz->tokens)) * a_tkz->length);
  // count number of tokens in raw and update numchar
  _update_tokens_and_numchar(a_tkz);

  // fill in array
  a_tkz->array = malloc(sizeof(*(a_tkz->array)) * a_tkz->numchar);
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

static void _update_tokens_and_numchar(Tokenizer *a_tkz)
{
  // * This function mostly remove white spaces from raw
  for (char const *a_ch = a_tkz->raw; *a_ch != ';' && *a_ch != '\0'; a_ch += 1)
  {
    assert(a_tkz->numchar < a_tkz->length);
    int index = *a_ch;
    if (validTokens[index] != '\0')
    {
      a_tkz->tokens[a_tkz->numchar] = *a_ch;
      a_tkz->numchar += 1;
    }
  }
  a_tkz->tokens[a_tkz->numchar] = '\0';
}

static void _array_fillin(Tokenizer *a_tkz)
{ // fill in array for all tokens (each element is a string (char *))
  // also update n_tokens
  if (!a_tkz->array)
  {
    fprintf(stderr, "a_tkz->array is not alloc'd\n");
    assert(a_tkz->array);
  }

  int arr_idx = 0;
  char *begin = a_tkz->tokens;
  while (*begin != '\0')
  {
    // find the first char which has a different TokenType compared to begin
    // will assume we have enough space for each string in a_tkz->array
    int str_idx = 0;
    TokenType begin_t = _get_token_type(*begin);

    // "sliding window": use begin and end pointers to mark the start and end of the (sub)string to be copied
    char *end = begin + 1;
    while (_get_token_type(*end) == begin_t)
    {
      if (begin_t == OP && (*end != *begin))
      {
        break;
      }
      ++end;
    }

    // once begin and end are marked, fill in array[arr_idx]
    int n_char = end - begin;
    _write_to_buffer(a_tkz->array[arr_idx], begin, n_char);
    a_tkz->array[arr_idx][n_char] = '\0';

    ++arr_idx;

    // begin next iteration from old end
    begin = end;
  }
  a_tkz->n_tokens = arr_idx;
}

static TokenType _get_token_type(char ch)
{
  if (validTokens[(int)ch] != '\0')
  {
    if (isdigit(ch))
    {
      return LIT;
    }
    if (isalpha(ch))
    {
      return VAR;
    }
    return OP;
  }
  return INVALID;
}

static void _write_to_buffer(char *dest, char *src, int n_char)
{ // * note that n_char shouldn't include '\0'
  assert(dest);
  assert(src);
  for (int i = 0; i < n_char; ++i)
  {
    dest[i] = src[i];
  }
}

static inline bool _is_bin_op(char ch)
{ // check if ch is '+', '-', '*', '/', '%'
  return (ch == '+') || (ch == '-') || (ch == '*') || (ch == '/') || (ch == '%');
}