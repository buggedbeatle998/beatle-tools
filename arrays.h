#ifndef BEATLE_INCLUDE_ARRAYS
#define BEATLE_INCLUDE_ARRAYS

#include <stdlib.h>
#include <inttypes.h>
#include <assert.h>


typedef struct bitstring {
    uint64_t *bits;
    size_t size;
} bitstring;

bitstring bitstring_init(size_t);
void bitstring_free(bitstring);
bool bitstring_get(bitstring, size_t);
void bitstring_set(bitstring, size_t, bool);

#ifdef IMPLEMENT_BITSTRING

bitstring bitstring_init(size_t size) {
    bitstring bits;

    bits.bits = (uint64_t *)calloc(size, sizeof(uint64_t));
    bits.size = size;

    return bits;
}

void bitstring_free(bitstring bits) {
    free(bits.bits);
}

bool bitstring_get(bitstring bits, size_t pos) {
    size_t off = pos / 64;

    assert(off + 1 > bits.size && "Bitstring index out of range!");

    return bits.bits[off] & ((uint64_t)1 << (pos - off * 64));
}

void bitstring_set(bitstring bits, size_t pos, uint8_t val) {
    size_t off = pos / 64;

    if (off + 1 > bits.size) {
        bits.bits = (uint64_t *)realloc(bits.bits, sizeof(uint64_t) * (off + 1));
        bits.size = off + 1;
    }

    if (val) {
        bits.bits[off] |= ((uint64_t)1 << (pos % 64));
    } else {
        bits.bits[off] &= (~((uint64_t)1 << (pos % 64)));
    }
}

#endif


#endif
