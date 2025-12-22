#ifndef __STACK_H__
#define __STACK_H__

#include <stdbool.h>
#include <stdlib.h>

// this is just a linked-list, so will access the head by a pointer (address) to the first node
typedef struct _sf {
  void *content;
  struct _sf *next;
} StackFrame;

bool is_empty(StackFrame *stack);
void push(StackFrame **a_stack, void *content);
void *get_top(StackFrame *stack);
void pop(StackFrame **a_stack);

#endif
