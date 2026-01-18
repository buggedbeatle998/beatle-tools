#include <stdio.h>
#include <stdbool.h>

#define IMPLEMENT_PQUEUE
#define BTL_USE_IO
#include "../queues.h"


bool min(const void *node, const void *min) {
    return *(int *)node < *(int *)min;
}

IMPLEMENT_TYPE_PQUEUE(int, char, ucpqueue);
int main(void) {
    ucpqueue arr = ucpqueue_init(&min);

    for (short i = 0; i < 100; ++i) {
        ucpqueue_push(arr, rand(), 'a' + (i % 25));
    }
    pqueue_Node *temp = ucpqueue_pushget(arr, rand(), 'z');
    ucpqueue_deckey(arr, temp, 0);

    while (!ucpqueue_is_empty(arr)) {
        printf("%c ", ucpqueue_peek(arr));
        printf("%d\n", ucpqueue_peek_priority(arr));
        ucpqueue_pop(arr);
    }

    ucpqueue_free(arr);
    
    return 0;
}
