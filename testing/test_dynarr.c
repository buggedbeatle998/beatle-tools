#include <stdio.h>
#include <stdbool.h>

#define IMPLEMENT_DYNARR
#include "../arrays.h"


IMPLEMENT_TYPE_DYNARR(short, sharray);
int main(void) {
    sharray arr = sharray_init(9);

    for (short i = 5; i < 80; ++i) {
        sharray_push(arr, i);
        printf("%d\n", sharray_peek(arr));
    }
    
    sharray_iset(arr, 60, sharray_iget(arr, 60) + 50);
    printf("\n");

    while (!sharray_is_empty(arr)) {
        printf("%d\n", sharray_peekpop(arr));
    }

    sharray_free(arr);
    
    return 0;
}
