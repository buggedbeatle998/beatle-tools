#ifndef BEATLE_INCLUDE_STACKS
#define BEATLE_INCLUDE_STACKS

#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

typedef size_t T;


// Stack
typedef struct {
    void *stack;
    size_t ele_size;
    size_t capacity;
    size_t size;
} Stack;

Stack *stack_init(size_t capacity, size_t ele_size);
void stack_free(Stack *stack);
void stack_push(Stack *stack, void *value);
void stack_pop(Stack *stack);
void *stack_peek(Stack *stack);
void * stack_peekpop(Stack *stack);
void stack_clear(Stack *stack);
bool stack_is_empty(Stack *stack);


static void *get_address(const void *array, size_t ele_size, size_t index) {
    return &((uint8_t *)array)[index * ele_size];
}


//#ifdef IMPLEMENT_STACK

Stack *stack_init(size_t ele_size, size_t capacity) {
    Stack *stack = malloc(sizeof(Stack));
    stack->stack = malloc(ele_size * capacity);
    stack->ele_size = ele_size;
    stack->capacity = capacity;
    stack->size = 0;

    return stack;
}

void stack_free(Stack *stack) {
    free(stack->stack);
    free(stack);
}

void stack_push(Stack *stack, void *value) {
    assert(stack->size < stack->capacity && "Stack overflow!");
    memcpy(stack->stack + stack->size++ * stack->ele_size, value, stack->ele_size);
}

void stack_pop(Stack *stack) {
    assert(stack->size > 0 && "Stack underflow!");
    --stack->size;
}

void *stack_peek(Stack *stack) {
    assert(stack->size > 0 && "Can not peek empty stack!");
    return stack->stack + (stack->size - 1) * stack->ele_size;
}

void *stack_peekpop(Stack *stack) {
    assert(stack->size > 0 && "Stack underflow!");
    return stack->stack + --stack->size * stack->ele_size;
}

void stack_clear(Stack *stack) {
    stack->size = 0;
}

bool stack_is_empty(Stack *stack) {
    return stack->size == 0;
}

#define IMPLEMENT_TYPE_STACK(type, handle) \
typedef struct { \
    Stack *stack; \
} handle; \
handle handle##_init     (size_t capacity) {return (handle){stack_init(sizeof(type), capacity)};} \
void   handle##_free     (handle stack) {stack_free(stack.stack);} \
void   handle##_push     (handle stack, type value) {stack_push(stack.stack, &value);} \
void   handle##_pop      (handle stack) {stack_pop(stack.stack);} \
type   handle##_peek     (handle stack) {return *(type *)stack_peek(stack.stack);} \
type   handle##_peekpop  (handle stack) {return *(type *)stack_peekpop(stack.stack);} \
void   handle##_clear    (handle stack) {stack_clear(stack.stack);} \
bool   handle##_is_empty (handle stack) {return stack_is_empty(stack.stack);} \

//#endif


#endif
