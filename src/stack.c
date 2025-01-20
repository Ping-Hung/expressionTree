#include "headers\stack.h"
#include <stdio.h>

bool is_empty(StackFrame *a_stack)
{
  return a_stack == NULL;
}

void push(StackFrame *a_stack, void *content)
{
  StackFrame *new_frame = malloc(sizeof(*new_frame));
  *new_frame = (StackFrame){.content = content, .next = a_stack};
  a_stack = new_frame;
}

void *pop(StackFrame *a_stack)
{
  if (a_stack)
  {
    StackFrame *temp = a_stack;
    void *content = a_stack->content;
    a_stack = a_stack->next;
    free(temp);
    return content;
  }
  fprintf(stderr, "%s in %s is wrong", __func__, __FILE__);
  return NULL;
}