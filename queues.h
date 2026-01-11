#ifndef BEATLE_INCLUDE_QUEUES
#define BEATLE_INCLUDE_QUEUES


#include <inttypes.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
typedef size_t T;


// Queue
typedef struct {
    T *queue;
    size_t capacity;
    size_t head;
    size_t tail;
    size_t size;
} Queue;

typedef struct {
    
} thing;

Queue *queue_init(size_t size);
void queue_free(Queue *queue, bool free_elements);
void queue_push(Queue *queue, T value);
void queue_pop(Queue *queue);
T queue_peek(Queue *queue);


// Dequeue
void dequeue_pushfirst(Queue *queue, T value);
void dequeue_poplast(Queue *queue);
T dequeue_peeklast(Queue *queue);


// Priority Queue
typedef struct Node {
    T priority;
    T value;
    struct Node *left;
    struct Node *right;
} Node;

void pqueue_push(Node *pqueue, T value, T priority);
uint64_t pqueue_pop();


#ifdef IMPLEMENT_DEQUEUE
    #define IMPLEMENT_QUEUE
#endif


#ifdef IMPLEMENT_QUEUE

Queue *queue_init(size_t capacity) {
    Queue *queue = (Queue *)malloc(sizeof(Queue));
    queue->queue = (T *)malloc(sizeof(T) * capacity);
    queue->capacity = capacity;
    queue->head = 0;
    queue->tail = 0;
    queue->size = 0;

    return queue;
}

void queue_free(Queue *queue, bool free_elements) {
    if (free_elements) {
        bool wrap = queue->tail < queue->head;
        size_t idx = queue->head;
        while (idx < queue->size && wrap ^ (idx < queue->tail)) {
            free((void *)queue->queue[idx]);
        }
    }

    free(queue->queue);
    free(queue);
}

void queue_push(Queue *queue, T value) {
    assert(queue->size < queue->capacity && "Popped from empty queue!");
    queue->queue[queue->tail] = value;
    if (++queue->tail == queue->capacity)
        queue->tail = 0;
    ++queue->size;
}

void queue_pop(Queue *queue) {
    assert(queue->size > 0 && "Popped from empty queue!");
    if (++queue->head == queue->capacity)
        queue->head = 0;
    --queue->size;
}

T queue_peek(Queue *queue) {
    assert(queue->size > 0 && "Peeked empty queue!");
    return queue->queue[queue->head];
}

#endif

#ifdef IMPLEMENT_DEQUEUE

void dequeue_pushfirst(Queue *queue, T value) {
    assert(queue->size < queue->capacity && "Popped from empty queue!");
    if (queue->head == 0)
        queue->head = queue->capacity - 1;
    else
        --queue->head;
    queue->queue[queue->head] = value;
    ++queue->size;
}

void dequeue_poplast(Queue *queue) {
    assert(queue->size > 0 && "Popped from empty queue!");
    if (queue->tail == 0)
        queue->tail = queue->capacity - 1;
    else
        --queue->tail;
    --queue->size;
}

T dequeue_peeklast(Queue *queue) {
    assert(queue->size > 0 && "Peeked empty queue!");
    if (queue->tail == 0)
        return queue->queue[queue->capacity - 1];
    return queue->queue[queue->tail - 1];
}

#endif

#ifdef IMPLEMENT_PRIORITY_QUEUE



#endif


#endif
