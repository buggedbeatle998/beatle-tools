#ifndef BEATLE_INCLUDE_SEQUENCES
#define BEATLE_INCLUDE_SEQUENCES


#include <stdint.h>
#include <stdbool.h>


// Fib
const static uint8_t tab32[32] = {
     0,  9,  1, 10, 13, 21,  2, 29,
    11, 14, 16, 18, 22, 25,  3, 30,
     8, 12, 20, 28, 15, 17, 24,  7,
    19, 27, 23,  6, 26,  5,  4, 31
};

static uint32_t log2_32(uint32_t value);
uint64_t fib(uint32_t n);


#ifdef IMPLEMENT_FIB

static uint32_t log2_32(uint32_t value) {
    value |= value >> 1;
    value |= value >> 2;
    value |= value >> 4;
    value |= value >> 8;
    value |= value >> 16;
    return tab32[(uint32_t)(value * 0x07C4ACDD) >> 27];
}

uint64_t fib(uint32_t n) {
    if (n > 1) {
        uint64_t m0 = 0;
        uint64_t m1 = 1;
        uint64_t t0;
        uint32_t i = log2_32(n);
        bool is_neg = true;

        while (i--) {
            t0 = m0 * m0;
            m0 = m1 * m1;
            m1 = m0 << 2;
            m1 -= t0;
            if (is_neg)
                m1 -= 2;
            else
                m1 += 2;
            m0 += t0;

            if ((is_neg = n & (1 << i)))
                m0 = m1 - m0;
            else
                m1 -= m0;
        }
        return m1;
    } else {
        return n;
    }
}

#endif


#endif
