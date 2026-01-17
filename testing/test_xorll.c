#include <stdio.h>
#include <stdbool.h>

#define IMPLEMENT_XORLL
#include "../arrays.h"


IMPLEMENT_TYPE_XORLL(float, fxor);
int main(void) {
    fxor arr = fxor_init();

    for (short i = 0; i < 100; ++i) {
        if (i & 1) {
            fxor_pushback(arr, i);
        } else {
            fxor_pushfront(arr, i);
        }
    }
   
    fxoriter iter = fxoriter_initfront(arr);
    printf("\n");

    while (true) {
        printf("%f\n", fxoriter_peekcurr(iter));
        if (fxoriter_next(iter))
            break;
    }

    fxoriter_free(iter);
    fxor_free(arr);
    
    return 0;
}
