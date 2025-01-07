#ifndef __TOKENIZER_H__
#define __TOKENIZER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_STR_LEN 1024
/*TokenStr is a "string" type that holds at most 1024 characters (including '\0')*/
typedef char TokensStr[MAX_STR_LEN];

/*There should be 3 kinds of token in this particular project: VARiables, LITerals, OPerators*/

typedef enum
{
  VAR,
  LIT,
  OP,
  INVALID
} TokenType;

// this should be a LUT (Look Up Table) for all valid tokens
// use extern so validTokens (global var) is usable (and identical) throughout all files
// that includes tokenizer.h
extern char validTokens[256];

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