#include <stdio.h>
#include <stdbool.h>

#define IMPLEMENT_DEQUEUE
#include "../queues.h"


IMPLEMENT_TYPE_DEQUEUE(int, cqueue);
int main(void) {
    cqueue arr = cqueue_init(10);

    for (short i = 0; i < 14; ++i) {
        cqueue_push(arr, i);
        printf("%d\n", cqueue_peek(arr));
        printf("%d\n", cqueue_peeklast(arr));
        if (i > 8)
            cqueue_pop(arr);
    }
    
    printf("\n");

    while (!cqueue_is_empty(arr)) {
        printf("%d\n", cqueue_peekpop(arr));
    }

    cqueue_free(arr);
    
    return 0;
}
