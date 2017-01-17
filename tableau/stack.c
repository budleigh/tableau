#include "stack.h"

void push(Stack *s, int val) {
	s->storage[s->size] = val;
	s->size++;
}

int pop(Stack *s) {
	// because this is a bespoke
	// stack, -1 represents an
	// 'empty' stack even tho its
	// also an int.
	if (s->size == 0) {
		return -1;
	}
	s->size--;
	return s->storage[s->size];
}
