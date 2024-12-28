#ifndef __TOKENIZER_H__
#define __TOKENIZER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <ctype.h>

typedef struct
{
  char const *raw;
  int length;
  char *tokens;
  int n_tokens;
} Tokenizer;

Tokenizer init_tokenizer(char const *raw);
void tokenize(Tokenizer *a_tkz);
void destroy_tokenizer(Tokenizer *a_tkz);

#endif