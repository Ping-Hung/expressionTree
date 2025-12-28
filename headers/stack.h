#ifndef __STACK_H__
#define __STACK_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Nothing but a linked list, so will access the head by a pointer (address) to the first node
typedef struct _sf {
	void *content;
	struct _sf *next;
} StackFrame;

static inline bool is_empty(StackFrame *stack)
{
	return stack == NULL;
}

static inline void push(StackFrame **a_stack, void *content)
{
	StackFrame *new_frame = malloc(sizeof(*new_frame));
	*new_frame = (StackFrame){.content = content, .next = *a_stack};
	*a_stack = new_frame;
}

static inline void *get_top(StackFrame *stack)
{
	if (!is_empty(stack)) {
		return stack->content;
	}
	fprintf(stderr, "%s:%s: trying to get top of an empty stack\n", __FILE__, __func__);
	return NULL;
}

static inline void pop(StackFrame **a_stack)
{
	if (!is_empty(*a_stack)) {
		StackFrame *tmp = *a_stack;
		*a_stack = (*a_stack)->next;
		free(tmp);
	}
}

#endif
