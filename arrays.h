#ifndef BEATLE_INCLUDE_ARRAYS
#define BEATLE_INCLUDE_ARRAYS

#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <assert.h>

typedef size_t T;


// Bitstring
typedef struct bitstring {
    uint64_t *bits;
    size_t size;
} bitstring;

bitstring bitstring_init(size_t);
void bitstring_free(bitstring);
bool bitstring_get(bitstring, size_t);
void bitstring_set(bitstring, size_t, bool);


// XORLL
typedef struct {
    T value;
    intptr_t XORptr;
} __btl_XORLL_Node;

typedef struct {
    intptr_t curr;
    intptr_t prev;
} XORLL_Iter;

typedef struct {
    intptr_t start;
    intptr_t end;
} XORLL;

XORLL *XORLL_init(void);
void XORLL_free(XORLL *xorll);
void XORLL_pushback(XORLL *xorll, T value);
void XORLL_pushfront(XORLL *xorll, T value);
void XORLL_popback(XORLL *xorll);
void XORLL_popfront(XORLL *xorll);
T XORLL_peekback(XORLL *xorll);
T XORLL_peekfront(XORLL *xorll);

XORLL_Iter *XORLLiter_initfront(XORLL *xorll);
XORLL_Iter *XORLLiter_initback(XORLL *xorll);
void XORLLiter_free(XORLL_Iter *iter);
bool XORLLiter_next(XORLL_Iter *iter);
bool XORLLiter_prev(XORLL_Iter *iter);
void XORLLiter_insertprev(XORLL_Iter *iter, T value);
void XORLLiter_removecurr(XORLL_Iter *iter);
T XORLiterL_peekcurr(XORLL_Iter *iter);


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

#ifdef IMPLEMENT_XORLL

XORLL *XORLL_init(void) {
    XORLL *xorll = malloc(sizeof(XORLL));
    xorll->start = (intptr_t)NULL;
    xorll->end = (intptr_t)NULL;

    return xorll;
}

void XORLL_free(XORLL *xorll) {
    intptr_t tmpptr = (intptr_t)NULL;
    intptr_t dummy = xorll->start;
    while (dummy != (intptr_t)NULL) {
        tmpptr ^= ((__btl_XORLL_Node *)dummy)->XORptr;
        tmpptr ^= dummy;
        dummy ^= tmpptr;
        tmpptr ^= dummy;
        free((__btl_XORLL_Node *)tmpptr);
    }

    free(xorll);
}

void XORLL_pushback(XORLL *xorll, T value) {
    __btl_XORLL_Node *temp = (__btl_XORLL_Node *)malloc(sizeof(__btl_XORLL_Node));
    temp->value = value;
    temp->XORptr = xorll->end;
    intptr_t tmpptr = (intptr_t)temp;
    if (xorll->end != (intptr_t)NULL) {
        ((__btl_XORLL_Node *)xorll->end)->XORptr ^= tmpptr;
    } else {
        xorll->start = tmpptr;
    }
    xorll->end = tmpptr;
}

void XORLL_pushfront(XORLL *xorll, T value) {
    __btl_XORLL_Node *temp = (__btl_XORLL_Node *)malloc(sizeof(__btl_XORLL_Node));
    temp->value = value;
    temp->XORptr = xorll->start;
    intptr_t tmpptr = (intptr_t)temp;
    if (xorll->start != (intptr_t)NULL) {
        ((__btl_XORLL_Node *)xorll->start)->XORptr ^= tmpptr;
    } else {
        xorll->end = tmpptr;
    }
    xorll->start = tmpptr;
}

void XORLL_popback(XORLL *xorll) {
    assert(xorll->end != (intptr_t)NULL);
    intptr_t tmpptr = xorll->end;
    xorll->end = ((__btl_XORLL_Node *)tmpptr)->XORptr;
    ((__btl_XORLL_Node *)xorll->end)->XORptr ^= tmpptr;
    free((__btl_XORLL_Node *)tmpptr);
}

void XORLL_popfront(XORLL *xorll) {
    assert(xorll->start != (intptr_t)NULL);
    intptr_t tmpptr = xorll->start;
    xorll->start = ((__btl_XORLL_Node *)tmpptr)->XORptr;
    ((__btl_XORLL_Node *)xorll->start)->XORptr ^= tmpptr;
    free((__btl_XORLL_Node *)tmpptr);
}

T XORLL_peekback(XORLL *xorll) {
    assert(xorll->end != (intptr_t)NULL);
    return ((__btl_XORLL_Node *)xorll->end)->value;
}

T XORLL_peekfront(XORLL *xorll) {
    assert(xorll->start != (intptr_t)NULL);
    return ((__btl_XORLL_Node *)xorll->start)->value;
}

XORLL_Iter *XORLLiter_initfront(XORLL *xorll) {
    XORLL_Iter *iter = (XORLL_Iter *)malloc(sizeof(XORLL_Iter));
    iter->curr = xorll->start;
    iter->prev = (intptr_t)NULL;

    return iter;
}

XORLL_Iter *XORLLiter_initback(XORLL *xorll) {
    XORLL_Iter *iter = (XORLL_Iter *)malloc(sizeof(XORLL_Iter));
    iter->curr = xorll->end;
    iter->prev = (intptr_t)NULL;

    return iter;
}

void XORLLiter_free(XORLL_Iter *iter) {
    free(iter);
}

bool XORLLiter_next(XORLL_Iter *iter) {
    iter->prev ^= ((__btl_XORLL_Node *)iter->curr)->XORptr;
    iter->prev ^= iter->curr;
    iter->curr ^= iter->prev;
    iter->prev ^= iter->curr;

    return iter->curr == (intptr_t)NULL;
}

bool XORLLiter_prev(XORLL_Iter *iter) {
    iter->curr ^= ((__btl_XORLL_Node *)iter->prev)->XORptr;
    iter->curr ^= iter->prev;
    iter->prev ^= iter->curr;
    iter->curr ^= iter->prev;

    return iter->curr == (intptr_t)NULL;
}

void XORLLiter_insertprev(XORLL_Iter *iter, T value) {
    assert(iter->prev != (intptr_t)NULL && "Can not insert end node!");
    __btl_XORLL_Node *temp = (__btl_XORLL_Node *)malloc(sizeof(__btl_XORLL_Node));
    temp->value = value;
    temp->XORptr = iter->curr ^ iter->prev;
    intptr_t tmpptr = (intptr_t)temp;
    if (iter->prev != (intptr_t)NULL) {
        ((__btl_XORLL_Node *)iter->prev)->XORptr ^= iter->curr ^ tmpptr;
    }
    ((__btl_XORLL_Node *)iter->prev)->XORptr ^= iter->curr ^ tmpptr;
}

void XORLLiter_removecurr(XORLL_Iter *iter) {
    intptr_t temp = iter->curr;
    iter->curr = iter->prev ^ ((__btl_XORLL_Node *)iter->curr)->XORptr;
    ((__btl_XORLL_Node *)iter->curr)->XORptr ^= temp ^ iter->prev;
    ((__btl_XORLL_Node *)iter->prev)->XORptr ^= temp ^ iter->curr;
    assert(iter->curr != (intptr_t)NULL && "Can not remove end node!");
    free((__btl_XORLL_Node *)temp);
}

T XORLiterL_peekcurr(XORLL_Iter *iter) {
    return ((__btl_XORLL_Node *)iter->curr)->value;
}

#endif


#endif
