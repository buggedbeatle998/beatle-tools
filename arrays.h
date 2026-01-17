#ifndef BEATLE_INCLUDE_ARRAYS
#define BEATLE_INCLUDE_ARRAYS

#ifndef HLOCAL
    #define HLOCAL static
#endif

#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>


// Bitstring
typedef struct {
    uint64_t *bits;
    size_t size;
} bitstring;

bitstring bitstring_init    (size_t size);
void      bitstring_free    (      bitstring bits);
bool      bitstring_get     (const bitstring bits, size_t index);
void      bitstring_set     (      bitstring bits, size_t index, bool value);


// Dynamic Array
typedef struct {
    void *array;
    size_t ele_size;
    size_t size;
    size_t capacity;
} Dynarr;

HLOCAL Dynarr *dynarr_init      (size_t min_capacity, size_t ele_size);
HLOCAL void    dynarr_free      (      Dynarr *dynarr);
HLOCAL void    dynarr_push      (      Dynarr *dynarr, const void *value);
HLOCAL void    dynarr_pop       (      Dynarr *dynarr);
HLOCAL void   *dynarr_peek      (const Dynarr *dynarr);
HLOCAL void   *dynarr_iget      (const Dynarr *dynarr, size_t i);
HLOCAL void    dynarr_iset      (      Dynarr *dynarr, size_t i, const void *value);
HLOCAL bool    dynarr_is_empty  (const Dynarr *dynarr);


// XORLL
typedef struct {
    void *value;
    intptr_t XORptr;
} __btl_xorll_Node;

typedef struct {
    intptr_t curr;
    intptr_t prev;
    size_t ele_size;
} XORLLIter;

typedef struct {
    intptr_t start;
    intptr_t end;
    size_t ele_size;
} XORLL;

HLOCAL XORLL *xorll_init        (size_t ele_size);
HLOCAL void   xorll_free        (      XORLL *xorll);
HLOCAL void   xorll_pushback    (      XORLL *xorll, const void *value);
HLOCAL void   xorll_pushfront   (      XORLL *xorll, const void *value);
HLOCAL void   xorll_popback     (      XORLL *xorll);
HLOCAL void   xorll_popfront    (      XORLL *xorll);
HLOCAL void  *xorll_peekback    (const XORLL *xorll);
HLOCAL void  *xorll_peekfront   (const XORLL *xorll);
HLOCAL bool   xorll_is_empty    (const XORLL *xorll);

HLOCAL XORLLIter  *xorlliter_initfront     (const XORLL *xorll);
HLOCAL XORLLIter  *xorlliter_initback      (const XORLL *xorll);
HLOCAL void        xorlliter_free          (      XORLLIter *iter);
HLOCAL bool        xorlliter_next          (      XORLLIter *iter);
HLOCAL bool        xorlliter_prev          (      XORLLIter *iter);
HLOCAL void        xorlliter_insertprev    (      XORLLIter *iter, const void *value);
HLOCAL void        xorlliter_removecurr    (      XORLLIter *iter);
HLOCAL void       *xorlliter_peekcurr      (const XORLLIter *iter);


#ifdef IMPLEMENT_BITSTRING
// start
    bitstring bitstring_init(size_t size) {
        bitstring bits;
    
        bits.bits = (uint64_t *)calloc(size, sizeof(uint64_t));
        bits.size = size;
    
        return bits;
    }
    
    void bitstring_free(bitstring bits) {
        free(bits.bits);
    }
    
    bool bitstring_get(bitstring bits, size_t index) {
        size_t off = index / 64;
    
        assert(off + 1 > bits.size && "Bitstring index out of range!");
    
        return bits.bits[off] & ((uint64_t)1 << (index - off * 64));
    }
    
    void bitstring_set(bitstring bits, size_t index, bool value) {
        size_t off = index / 64;
    
        if (off + 1 > bits.size) {
            bits.bits = (uint64_t *)realloc(bits.bits, sizeof(uint64_t) * (off + 1));
            bits.size = off + 1;
        }
    
        if (value) {
            bits.bits[off] |= ((uint64_t)1 << (index % 64));
        } else {
            bits.bits[off] &= (~((uint64_t)1 << (index % 64)));
        }
    }
// end
#endif

#ifdef IMPLEMENT_DYNARR
// start    
    HLOCAL Dynarr *dynarr_init(size_t min_capacity, size_t ele_size) {
        Dynarr *dynarr = (Dynarr *)malloc(sizeof(Dynarr));
        
        --min_capacity;
        min_capacity |= min_capacity >> 1;
        min_capacity |= min_capacity >> 2;
        min_capacity |= min_capacity >> 4;
        min_capacity |= min_capacity >> 8;
        min_capacity |= min_capacity >> 16;
        min_capacity |= min_capacity >> 32;
        ++min_capacity;
    
        dynarr->array = malloc(ele_size * min_capacity);
        dynarr->ele_size = ele_size;
        dynarr->size = 0;
        dynarr->capacity = min_capacity;
    
        return dynarr;
    }
    
    HLOCAL void dynarr_free(Dynarr *dynarr) {
        free(dynarr->array);
        free(dynarr);
    }
    
    HLOCAL void dynarr_push(Dynarr *dynarr, const void *value) {
        if (dynarr->size >= dynarr->capacity) {
            dynarr->capacity <<= 1;
            dynarr->array = realloc(dynarr->array, dynarr->ele_size * dynarr->capacity);
        }
        mempcpy(dynarr->array + dynarr->size++ * dynarr->ele_size, value, dynarr->ele_size);
    }
    
    HLOCAL void dynarr_pop(Dynarr *dynarr) {
        assert(dynarr->size > 0 && "Can not pop from empty array!");
        --dynarr->size;
    }
    
    HLOCAL void *dynarr_peek(const Dynarr *dynarr) {
        assert(dynarr->size > 0 && "Can not peek empty array!");
        return dynarr->array + (dynarr->size - 1) * dynarr->ele_size;
    }
    
    HLOCAL void *dynarr_peekpop(Dynarr *dynarr) {
        assert(dynarr->size > 0 && "Can not peekpop empty array!");
        return dynarr->array + (--dynarr->size) * dynarr->ele_size;
    }
    
    HLOCAL void *dynarr_iget(const Dynarr *dynarr, size_t i) {
        assert(i < dynarr->size && "Index out of range!");
        return dynarr->array + i * dynarr->ele_size;
    }
    
    HLOCAL void dynarr_iset(Dynarr *dynarr, size_t i, const void *value) {
        assert(i < dynarr->size && "Index out of range!");
        mempcpy(dynarr->array + i * dynarr->ele_size, value, dynarr->ele_size);
    }
    
    HLOCAL bool dynarr_is_empty(const Dynarr *dynarr) {
        return dynarr->size == 0;
    }
  
// generic macro

    #define IMPLEMENT_TYPE_DYNARR(type, handle) \
    typedef struct { \
        Dynarr *dynarr; \
    } handle; \
    static inline handle handle##_init     (size_t capacity) {return (handle){dynarr_init(sizeof(type), capacity)};} \
    static inline void   handle##_free     (handle dynarr) {dynarr_free(dynarr.dynarr);} \
    static inline void   handle##_push     (handle dynarr, type value) {dynarr_push(dynarr.dynarr, &value);} \
    static inline void   handle##_pop      (handle dynarr) {dynarr_pop(dynarr.dynarr);} \
    static inline type   handle##_peek     (handle dynarr) {return *(type *)dynarr_peek(dynarr.dynarr);} \
    static inline type   handle##_peekpop  (handle dynarr) {return *(type *)dynarr_peekpop(dynarr.dynarr);} \
    static inline type   handle##_iget     (handle dynarr, size_t i) {return *(type *)dynarr_iget(dynarr.dynarr, i);} \
    static inline void   handle##_iset     (handle dynarr, size_t i, type value) {dynarr_iset(dynarr.dynarr, i, &value);} \
    static inline bool   handle##_is_empty (handle dynarr) {return dynarr_is_empty(dynarr.dynarr);} \
//begin
#endif

#ifdef IMPLEMENT_XORLL
// start
    HLOCAL XORLL *xorll_init(size_t ele_size) {
        XORLL *xorll = malloc(sizeof(XORLL));
        xorll->start = (intptr_t)NULL;
        xorll->end = (intptr_t)NULL;
        xorll->ele_size = ele_size;
    
        return xorll;
    }
    
    HLOCAL void xorll_free(XORLL *xorll) {
        intptr_t tmpptr = (intptr_t)NULL;
        intptr_t dummy = xorll->start;
        while (dummy != (intptr_t)NULL) {
            tmpptr ^= ((__btl_xorll_Node *)dummy)->XORptr;
            tmpptr ^= dummy;
            dummy ^= tmpptr;
            tmpptr ^= dummy;
            free(((__btl_xorll_Node *)tmpptr)->value);
            free((__btl_xorll_Node *)tmpptr);
        }
    
        free(xorll);
    }

    HLOCAL void xorll_pushback(XORLL *xorll, const void *value) {
        __btl_xorll_Node *temp = malloc(sizeof(__btl_xorll_Node));
        temp->value = malloc(xorll->ele_size);
        mempcpy(temp->value, value, xorll->ele_size);
        temp->XORptr = xorll->end;

        intptr_t tmpptr = (intptr_t)temp;
        if (xorll->end != (intptr_t)NULL) {
            ((__btl_xorll_Node *)xorll->end)->XORptr ^= tmpptr;
        } else {
            xorll->start = tmpptr;
        }
        xorll->end = tmpptr;
    }
    
    HLOCAL void xorll_pushfront(XORLL *xorll, const void *value) {
        __btl_xorll_Node *temp = malloc(sizeof(__btl_xorll_Node));
        temp->value = malloc(xorll->ele_size);
        mempcpy(temp->value, value, xorll->ele_size);
        temp->XORptr = xorll->start;

        intptr_t tmpptr = (intptr_t)temp;
        if (xorll->start != (intptr_t)NULL) {
            ((__btl_xorll_Node *)xorll->start)->XORptr ^= tmpptr;
        } else {
            xorll->end = tmpptr;
        }
        xorll->start = tmpptr;
    }
    
    HLOCAL void xorll_popback(XORLL *xorll) {
        assert(xorll->end != (intptr_t)NULL);
        intptr_t tmpptr = xorll->end;
        xorll->end = ((__btl_xorll_Node *)tmpptr)->XORptr;
        ((__btl_xorll_Node *)xorll->end)->XORptr ^= tmpptr;
        free(((__btl_xorll_Node *)tmpptr)->value);
        free((__btl_xorll_Node *)tmpptr);
    }
    
    HLOCAL void xorll_popfront(XORLL *xorll) {
        assert(xorll->start != (intptr_t)NULL);
        intptr_t tmpptr = xorll->start;
        xorll->start = ((__btl_xorll_Node *)tmpptr)->XORptr;
        ((__btl_xorll_Node *)xorll->start)->XORptr ^= tmpptr;
        free(((__btl_xorll_Node *)tmpptr)->value);
        free((__btl_xorll_Node *)tmpptr);
    }
    
    HLOCAL void *xorll_peekback(const XORLL *xorll) {
        assert(xorll->end != (intptr_t)NULL);
        return ((__btl_xorll_Node *)xorll->end)->value;
    }
    
    HLOCAL void *xorll_peekfront(const XORLL *xorll) {
        assert(xorll->start != (intptr_t)NULL);
        return ((__btl_xorll_Node *)xorll->start)->value;
    }

    HLOCAL bool xorll_is_empty(const XORLL *xorll) {
        return xorll->start == (intptr_t)NULL;
    }
    
    HLOCAL XORLLIter *xorlliter_initfront(const XORLL *xorll) {
        XORLLIter *iter = malloc(sizeof(XORLLIter));
        iter->curr = xorll->start;
        iter->prev = (intptr_t)NULL;
        iter->ele_size = xorll->ele_size;
    
        return iter;
    }
    
    HLOCAL XORLLIter *xorlliter_initback(const XORLL *xorll) {
        XORLLIter *iter = malloc(sizeof(XORLLIter));
        iter->curr = xorll->end;
        iter->prev = (intptr_t)NULL;
        iter->ele_size = xorll->ele_size;
    
        return iter;
    }
    
    HLOCAL void xorlliter_free(XORLLIter *iter) {
        free(iter);
    }
    
    HLOCAL bool xorlliter_next(XORLLIter *iter) {
        iter->prev ^= ((__btl_xorll_Node *)iter->curr)->XORptr;
        iter->prev ^= iter->curr;
        iter->curr ^= iter->prev;
        iter->prev ^= iter->curr;
    
        return iter->curr == (intptr_t)NULL;
    }
    
    HLOCAL bool xorlliter_prev(XORLLIter *iter) {
        iter->curr ^= ((__btl_xorll_Node *)iter->prev)->XORptr;
        iter->curr ^= iter->prev;
        iter->prev ^= iter->curr;
        iter->curr ^= iter->prev;
    
        return iter->curr == (intptr_t)NULL;
    }
    
    HLOCAL void xorlliter_insertprev(XORLLIter *iter, const void *value) {
        assert(iter->prev != (intptr_t)NULL && "Can not insert end node!");
        __btl_xorll_Node *temp = malloc(sizeof(__btl_xorll_Node));
        temp->value = malloc(iter->ele_size);
        mempcpy(temp->value, value, iter->ele_size);
        temp->XORptr = iter->curr ^ iter->prev;

        intptr_t tmpptr = (intptr_t)temp;
        if (iter->prev != (intptr_t)NULL) {
            ((__btl_xorll_Node *)iter->prev)->XORptr ^= iter->curr ^ tmpptr;
        }
        ((__btl_xorll_Node *)iter->prev)->XORptr ^= iter->curr ^ tmpptr;
    }
    
    HLOCAL void xorlliter_removecurr(XORLLIter *iter) {
        intptr_t temp = iter->curr;
        iter->curr = iter->prev ^ ((__btl_xorll_Node *)iter->curr)->XORptr;
        ((__btl_xorll_Node *)iter->curr)->XORptr ^= temp ^ iter->prev;
        ((__btl_xorll_Node *)iter->prev)->XORptr ^= temp ^ iter->curr;
        assert(iter->curr != (intptr_t)NULL && "Can not remove end node!");
        free(((__btl_xorll_Node *)temp)->value);
        free((__btl_xorll_Node *)temp);
    }
    
    HLOCAL void *xorlliter_peekcurr(const XORLLIter *iter) {
        return ((__btl_xorll_Node *)iter->curr)->value;
    }

// generic macro

    #define IMPLEMENT_TYPE_XORLL(type, handle) \
    typedef struct { \
        XORLL *xorll; \
    } handle; \
    static inline handle handle##_init      (void) {return (handle){xorll_init(sizeof(type))};} \
    static inline void   handle##_free      (handle xorll) {xorll_free(xorll.xorll);} \
    static inline void   handle##_pushback  (handle xorll, type value) {xorll_pushback(xorll.xorll, &value);} \
    static inline void   handle##_pushfront (handle xorll, type value) {xorll_pushfront(xorll.xorll, &value);} \
    static inline void   handle##_popback   (handle xorll) {xorll_popback(xorll.xorll);} \
    static inline void   handle##_popfront  (handle xorll) {xorll_popfront(xorll.xorll);} \
    static inline type   handle##_peekback  (handle xorll) {return *(type *)xorll_peekback(xorll.xorll);} \
    static inline type   handle##_peekfront (handle xorll) {return *(type *)xorll_peekfront(xorll.xorll);} \
    static inline bool   handle##_is_empty  (handle xorll) {return xorll_is_empty(xorll.xorll);} \
    typedef struct { \
        XORLLIter *iter; \
    } handle##iter; \
    static inline handle##iter handle##iter##_initfront  (handle xorll) {return (handle##iter){xorlliter_initfront(xorll.xorll)};} \
    static inline handle##iter handle##iter##_initback   (handle xorll) {return (handle##iter){xorlliter_initback(xorll.xorll)};} \
    static inline void         handle##iter##_free       (handle##iter iter) {xorlliter_free(iter.iter);} \
    static inline bool         handle##iter##_next       (handle##iter iter) {return xorlliter_next(iter.iter);} \
    static inline bool         handle##iter##_prev       (handle##iter iter) {return xorlliter_prev(iter.iter);} \
    static inline void         handle##iter##_insertprev (handle##iter iter, type value) {xorlliter_insertprev(iter.iter, &value);} \
    static inline void         handle##iter##_removecurr (handle##iter iter) {xorlliter_removecurr(iter.iter);} \
    static inline type         handle##iter##_peekcurr   (handle##iter iter) {return *(type *)xorlliter_peekcurr(iter.iter);} \
// end
#endif


#endif
