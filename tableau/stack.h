#ifndef STACK
#define STACK

#define STACK_SIZE 64

typedef struct _Stack Stack;

// just a basic integer stack
// for use in the Bucket caches.
struct _Stack {
	int storage[STACK_SIZE];
	int size;
};

void push(Stack *s, int val);
int  pop(Stack *s);

#endif
