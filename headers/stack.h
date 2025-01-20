#ifndef __STACK_H__
#define __STACK_H__

#include <stdbool.h>
#include <stdlib.h>

typedef struct _sf
{
  void *content;
  struct _sf *next;
} StackFrame;

bool is_empty(StackFrame *a_stack);
void push(StackFrame *a_stack, void *content);
void *pop(StackFrame *a_stack);

#endif
