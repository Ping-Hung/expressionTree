#include "../headers/stack.h"
#include <stdio.h>

bool is_empty(StackFrame *a_stack)
{
  return a_stack == NULL;
}

void push(StackFrame **a_stack, void *content)
{
  StackFrame *new_frame = malloc(sizeof(*new_frame));
  *new_frame = (StackFrame){.content = content, .next = *a_stack};
  *a_stack = new_frame;
}

void *get_top(StackFrame *a_stack)
{
  if (!is_empty(a_stack))
  {
    return a_stack->content;
  }
  fprintf(stderr, "%s:%s: trying to get top of an empty stack\n", __FILE__, __func__);
  return NULL;
}

void pop(StackFrame **a_stack)
{
  if (!is_empty(*a_stack))
  {
    StackFrame *tmp = *a_stack;
    *a_stack = (*a_stack)->next;
    free(tmp);
  }
  else
  {
    fprintf(stderr, "%s:%s: popping from empty stack", __FILE__, __func__);
  }
}