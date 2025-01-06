#ifndef __TOKENIZER_H__
#define __TOKENIZER_H__

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_STR_LEN 1024
/*TokenStr is a "string" type that holds at most 1024 characters (including '\0')*/
typedef char TokensStr[MAX_STR_LEN];

// this should be a LUT (Look Up Table) for all valid tokens
char validTokens[256] = {'\0'};

typedef struct
{
  char const *raw;
  int length;
  char *tokens;
  int n_tokens;
  TokensStr *array;
} Tokenizer;

Tokenizer init_tokenizer(char const *raw);
void tokenize(Tokenizer *a_tkz);
void destroy_tokenizer(Tokenizer *a_tkz);

#endif