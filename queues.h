#ifndef BEATLE_INCLUDE_QUEUES
#define BEATLE_INCLUDE_QUEUES

#ifndef HLOCAL
    #define HLOCAL static
#endif

#include <inttypes.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#ifdef BTL_USE_IO
#include <stdio.h>
#endif


// Queue
typedef struct {
    void *queue;
    size_t capacity;
    size_t head;
    size_t tail;
    size_t size;
    size_t ele_size;
} Queue;

HLOCAL Queue *queue_init     (size_t capacity, size_t ele_size);
HLOCAL void   queue_free     (      Queue *queue);
HLOCAL void   queue_push     (      Queue *queue, const void *value);
HLOCAL void   queue_pop      (      Queue *queue);
HLOCAL void  *queue_peek     (const Queue *queue);
HLOCAL void  *queue_peekpop  (      Queue *queue);
HLOCAL void   queue_clear    (      Queue *queue);
HLOCAL bool   queue_is_empty (const Queue *queue);


// Dequeue
HLOCAL void  dequeue_pushfirst   (      Queue *queue, const void *value);
HLOCAL void  dequeue_poplast     (      Queue *queue);
HLOCAL void *dequeue_peeklast    (const Queue *queue);
HLOCAL void *dequeue_peekpoplast (      Queue *queue);


// Priority Queue
typedef struct pqueue_Node {
    void *priority;
    void *value;

    struct pqueue_Node *parent;
    struct pqueue_Node *children;
    struct pqueue_Node *next;
    struct pqueue_Node *prev;

    uint8_t degree;
    uint8_t time;
} pqueue_Node;

typedef struct {
    bool (*higher)(const void *, const void *);
    pqueue_Node *min;
    pqueue_Node *degrees[64];
    size_t key_size;
    size_t ele_size;
} PQueue;

HLOCAL void __btl_pqNode_insert          (pqueue_Node *llist_node, pqueue_Node *start, pqueue_Node *end);
HLOCAL void __btl_pqNode_remove          (pqueue_Node *start, bool _free);
HLOCAL void __btl_pqNode_replace         (pqueue_Node *llist_node, pqueue_Node *start, pqueue_Node *end);
HLOCAL pqueue_Node *__btl_pqNode_combine (bool (*higher)(const void *, const void *), pqueue_Node *node1, pqueue_Node *node2);

HLOCAL void __btl_output_heap(pqueue_Node *root);

HLOCAL void __btl_pqueue_cut     (PQueue *pqueue, pqueue_Node *node);
HLOCAL void __btl_pqueue_collect (PQueue *pqueue);

HLOCAL PQueue      *pqueue_init          (bool (*higher)(const void *node, const void *min), size_t key_size, size_t ele_size);
HLOCAL void         pqueue_free          (      PQueue *pqueue);
HLOCAL void         pqueue_push          (      PQueue *pqueue, const void *priority, const void *value);
HLOCAL pqueue_Node *pqueue_pushget       (      PQueue *pqueue, const void *priority, const void *value);
HLOCAL void         pqueue_pop           (      PQueue *pqueue);
HLOCAL void        *pqueue_peek          (const PQueue *pqueue);
HLOCAL void        *pqueue_peek_priority (const PQueue *pqueue);
HLOCAL bool         pqueue_is_empty      (const PQueue *pqueue);
HLOCAL void         pqueue_deckey        (      PQueue *pqueue, pqueue_Node *node, const void *new_priority);


#ifdef IMPLEMENT_DEQUEUE
    #define IMPLEMENT_QUEUE
#endif


#ifdef IMPLEMENT_QUEUE
// start
    HLOCAL Queue *queue_init(size_t capacity, size_t ele_size) {
        Queue *queue = malloc(sizeof(Queue));
        queue->queue = malloc(ele_size * capacity);
        queue->capacity = capacity;
        queue->head = 0;
        queue->tail = 0;
        queue->size = 0;
        queue->ele_size = ele_size;
    
        return queue;
    }
    
    HLOCAL void queue_free(Queue *queue) {
        free(queue->queue);
        free(queue);
    }
    
    HLOCAL void queue_push(Queue *queue, const void *value) {
        assert(queue->size < queue->capacity && "Queue overflow!");
        mempcpy(queue->queue + queue->tail * queue->ele_size, value, queue->ele_size);
        if (++queue->tail >= queue->capacity)
            queue->tail = 0;
        ++queue->size;
    }
    
    HLOCAL void queue_pop(Queue *queue) {
        assert(queue->size > 0 && "Can not pop from empty queue!");
        if (++queue->head >= queue->capacity)
            queue->head = 0;
        --queue->size;
    }
    
    HLOCAL void *queue_peek(const Queue *queue) {
        assert(queue->size > 0 && "Can not peek empty queue!");
        return queue->queue + queue->head * queue->ele_size;
    }

    HLOCAL void *queue_peekpop(Queue *queue) {
        assert(queue->size > 0 && "Can not peekpop empty queue!");
        void *temp = queue->queue + queue->head * queue->ele_size;
        if (++queue->head >= queue->capacity)
            queue->head = 0;
        --queue->size;
        return temp;
    }

    HLOCAL void queue_clear(Queue *queue) {
        queue->head = 0;
        queue->tail = 0;
        queue->size = 0;
    }

    HLOCAL bool queue_is_empty(const Queue *queue) {
        return queue->size == 0;
    }

// generic macro

    #define IMPLEMENT_TYPE_QUEUE(type, handle) \
    typedef struct { \
        Queue *queue; \
    } handle; \
    static inline handle handle##_init     (size_t capacity) {return (handle){queue_init(capacity, sizeof(type))};} \
    static inline void   handle##_free     (handle queue) {queue_free(queue.queue);} \
    static inline void   handle##_push     (handle queue, type value) {queue_push(queue.queue, &value);} \
    static inline void   handle##_pop      (handle queue) {queue_pop(queue.queue);} \
    static inline type   handle##_peek     (handle queue) {return *(type *)queue_peek(queue.queue);} \
    static inline type   handle##_peekpop  (handle queue) {return *(type *)queue_peekpop(queue.queue);} \
    static inline void   handle##_clear    (handle queue) {queue_clear(queue.queue);} \
    static inline bool   handle##_is_empty (handle queue) {return queue_is_empty(queue.queue);}
// end
#endif

#ifdef IMPLEMENT_DEQUEUE
// start
    HLOCAL void dequeue_pushfirst(Queue *queue, const void *value) {
        assert(queue->size < queue->capacity && "Queue overflow!");
        if (queue->head == 0)
            queue->head = queue->capacity - 1;
        else
            --queue->head;
        mempcpy(queue->queue + queue->head * queue->ele_size, value, queue->ele_size);
        ++queue->size;
    }
    
    HLOCAL void dequeue_poplast(Queue *queue) {
        assert(queue->size > 0 && "Can not pop from empty queue!");
        if (queue->tail == 0)
            queue->tail = queue->capacity - 1;
        else
            --queue->tail;
        --queue->size;
    }
    
    HLOCAL void *dequeue_peeklast(const Queue *queue) {
        assert(queue->size > 0 && "Can not peek empty queue!");
        if (queue->tail == 0)
            return queue->queue + (queue->capacity - 1) * queue->ele_size;
        return queue->queue + (queue->tail - 1) * queue->ele_size;
    }

    HLOCAL void *dequeue_peekpoplast(Queue *queue) {
        assert(queue->size > 0 && "Can not peekpop empty queue!");
        if (queue->tail == 0)
            queue->tail = queue->capacity - 1;
        else
            --queue->tail;
        --queue->size;
        return queue->queue + queue->tail * queue->ele_size;
    }

// generic macro

    #define IMPLEMENT_TYPE_DEQUEUE(type, handle) \
    IMPLEMENT_TYPE_QUEUE(type, handle); \
    static inline void   handle##_pushfirst   (handle queue, type value) {dequeue_pushfirst(queue.queue, &value);} \
    static inline void   handle##_poplast     (handle queue) {dequeue_poplast(queue.queue);} \
    static inline type   handle##_peeklast    (handle queue) {return *(type *)dequeue_peeklast(queue.queue);} \
    static inline type   handle##_peekpoplast (handle queue) {return *(type *)dequeue_peekpoplast(queue.queue);}
// end
#endif

#ifdef IMPLEMENT_PQUEUE
// start
    HLOCAL void __btl_pqNode_insert(pqueue_Node *llist_node, pqueue_Node *start, pqueue_Node *end) {
        end->next = llist_node->next;
        llist_node->next->prev = end;
        start->prev = llist_node;
        llist_node->next = start;
    }
    
    HLOCAL void __btl_pqNode_remove(pqueue_Node *llist_node, bool _free) {
        assert(llist_node->next != llist_node && "Can not delete last node in linked list!");
        llist_node->prev->next = llist_node->next;
        llist_node->next->prev = llist_node->prev;
        if (_free) {
            free(llist_node->priority);
            free(llist_node->value);
            free(llist_node);
        }
    }
    
    HLOCAL void __btl_pqNode_replace(pqueue_Node *llist_node, pqueue_Node *start, pqueue_Node *end) {
        assert(llist_node->next != llist_node && "Can not replace last node in linked list!");
        end->next = llist_node->next;
        llist_node->next->prev = end;
        start->prev = llist_node->prev;
        llist_node->prev->next = start;
        free(llist_node->priority);
        free(llist_node->value);
        free(llist_node);
    }
    
    HLOCAL pqueue_Node *__btl_pqNode_combine(bool (*higher)(const void *node, const void *min), pqueue_Node *node1, pqueue_Node *node2) {
        if (higher(node2->priority, node1->priority)) {
            node1 = (pqueue_Node *)((uintptr_t)node1 ^ (uintptr_t)node2);
            node2 = (pqueue_Node *)((uintptr_t)node1 ^ (uintptr_t)node2);
            node1 = (pqueue_Node *)((uintptr_t)node1 ^ (uintptr_t)node2);
        }
    
        ++node1->degree;
        node2->parent = node1;
        node2->time = 0;
        __btl_pqNode_remove(node2, false);
        if (node1->children == NULL) {
            node1->children = node2;
            node2->prev = node2->next = node2;
        } else {
            __btl_pqNode_insert(node1->children, node2, node2);
        }
    
        return node1;
    }
    
    #ifdef BTL_USE_IO
    void __btl_output_heap(pqueue_Node *root) {
        pqueue_Node *dummy = root;
        while (true) {
            printf("%lld", dummy->priority);
            if (dummy->children != NULL) {
                printf("(");
                __btl_output_heap(dummy->children);
                printf(")");
            }
            
            dummy = dummy->next;
            if (dummy == root)
                break;
            printf(" ");
        }
    }
    #endif
    
    HLOCAL void __btl_pqueue_cut(PQueue *pqueue, pqueue_Node *node) {
       assert(node->parent != NULL && "Can not cut root node!");
        node->time = 1;
        pqueue_Node *parent = node->parent;
    
        if (node->next == node) {
            parent->children = NULL;
        } else {
            if (parent->children == node) {
                parent->children = node->next;
            }
            __btl_pqNode_remove(node, false);
        }
        
        __btl_pqNode_insert(pqueue->min, node, node);
    
        if (!parent->time) {
            parent->time = 2;
        } else if (parent->time == 2) {
            __btl_pqueue_cut(pqueue, parent);
        }
    }
    
    HLOCAL void __btl_pqueue_collect(PQueue *pqueue) {
        memset(pqueue->degrees, (size_t)NULL, 64);
        pqueue_Node *root = pqueue->min;
        pqueue_Node *temp;
        pqueue_Node *entry;
        pqueue_Node *nroot = NULL;
        bool done = false;
    
        while (true) {
            temp = root;
            root = root->next;
            done = pqueue->degrees[root->degree] == root;
    
            while ((entry = pqueue->degrees[temp->degree]) != NULL && entry != temp) {
                pqueue->degrees[temp->degree] = NULL;
                temp = __btl_pqNode_combine(pqueue->higher, temp, entry);
            }
            pqueue->degrees[temp->degree] = temp;

            if (done)
                break;
        }
        pqueue->min = temp;
    }
    
    HLOCAL PQueue *pqueue_init(bool (*higher)(const void *node, const void *min), size_t key_size, size_t ele_size) {
        PQueue *pqueue = malloc(sizeof(PQueue));
        pqueue->higher = higher;
        pqueue->min = NULL;
        pqueue->key_size = key_size;
        pqueue->ele_size = ele_size;
    
        return pqueue;
    }
    
    HLOCAL void pqueue_free(PQueue *pqueue) {
        while(!pqueue_is_empty(pqueue))
            pqueue_pop(pqueue);
        free(pqueue);
    }
    
    HLOCAL inline void pqueue_push(PQueue *pqueue, const void *priority, const void *value) {
        pqueue_pushget(pqueue, priority, value);
    }
    
    HLOCAL pqueue_Node *pqueue_pushget(PQueue *pqueue, const void *priority, const void *value) {
        pqueue_Node *temp = (pqueue_Node *)malloc(sizeof(pqueue_Node));
        temp->priority = malloc(pqueue->key_size);
        mempcpy(temp->priority, priority, pqueue->key_size);
        temp->value = malloc(pqueue->ele_size);
        mempcpy(temp->value, value, pqueue->ele_size);
        temp->children = NULL;
        temp->parent = NULL;
        temp->degree = 0;
        temp->time = 1;

        if (pqueue->min == NULL) {
            temp->next = temp->prev = temp;
            pqueue->min = temp;
        } else {
            __btl_pqNode_insert(pqueue->min, temp, temp);
            if (pqueue->higher(temp->priority, pqueue->min->priority))
                pqueue->min = temp;
        }
    
        return temp;
    }
    
    HLOCAL void pqueue_pop(PQueue *pqueue) {
        assert(pqueue->min != NULL && "Can not pop from empty pqueue!");
        pqueue_Node *min_root = pqueue->min;
        pqueue_Node *node = min_root->children;
    
        if (node == NULL) {
            if (min_root->next == min_root) {
                pqueue->min = NULL;
            } else {
                pqueue->min = min_root->next;
                __btl_pqNode_remove(min_root, true);
            }
        } else {
            pqueue_Node *temp = node;
            for (size_t i = 0; i < min_root->degree; ++i) {
                node->parent = NULL;
                node->time = 1;
                node = node->next;
            }
            assert(node == temp);
    
            if (min_root->next == min_root) {
                pqueue->min = node;
            } else {
                pqueue->min = min_root->next;
                __btl_pqNode_replace(min_root, node, node->prev);
            }
        }
        
        if (pqueue->min != NULL) {
            __btl_pqueue_collect(pqueue);
            min_root = pqueue->min;
            node = min_root->next;
            while (true) {
                if (pqueue->higher(node->priority, pqueue->min->priority))
                    pqueue->min = node;
                
                node = node->next;
                if (node == min_root)
                    break;
            }
        }
    }
    
    HLOCAL void *pqueue_peek(const PQueue *pqueue) {
        assert(pqueue->min != NULL && "Can not peek empty pqueue!");
        return pqueue->min->value;
    }
    
    HLOCAL void *pqueue_peek_priority(const PQueue *pqueue) {
        assert(pqueue->min != NULL && "Can not peek empty pqueue!");
        return pqueue->min->priority;
    }
    
    HLOCAL bool pqueue_is_empty(const PQueue *pqueue) {
        return pqueue->min == NULL;
    }
    
    HLOCAL void pqueue_deckey(PQueue *pqueue, pqueue_Node *node, const void *new_priority) {
        mempcpy(node->priority, new_priority, pqueue->key_size);
        if (node->parent != NULL && pqueue->higher(new_priority, node->parent->priority)) {
            __btl_pqueue_cut(pqueue, node);
        }
    
        if (pqueue->higher(new_priority, pqueue->min->priority)) {
            pqueue->min = node;
        }
    }

// generic macro
    
    #define IMPLEMENT_TYPE_PQUEUE(keyt, elet, handle) \
    typedef struct { \
        PQueue *pqueue; \
    } handle; \
    static inline handle       handle##_init          (bool (*higher)(const void *node, const void *min)) {return (handle){pqueue_init(higher, sizeof(keyt), sizeof(elet))};} \
    static inline void         handle##_free          (handle pqueue) {pqueue_free(pqueue.pqueue);} \
    static inline void         handle##_push          (handle pqueue, keyt priority, elet value) {pqueue_push(pqueue.pqueue, &priority, &value);} \
    static inline pqueue_Node *handle##_pushget       (handle pqueue, keyt priority, elet value) {return pqueue_pushget(pqueue.pqueue, &priority, &value);} \
    static inline void         handle##_pop           (handle pqueue) {pqueue_pop(pqueue.pqueue);} \
    static inline elet         handle##_peek          (handle pqueue) {return *(elet *)pqueue_peek(pqueue.pqueue);} \
    static inline keyt         handle##_peek_priority (handle pqueue) {return *(keyt *)pqueue_peek_priority(pqueue.pqueue);} \
    static inline bool         handle##_is_empty      (handle pqueue) {return pqueue_is_empty(pqueue.pqueue);} \
    static inline void         handle##_deckey        (handle pqueue, pqueue_Node *node, keyt new_priority) {pqueue_deckey(pqueue.pqueue, node, &new_priority);}
// end
#endif


#endif
