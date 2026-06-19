#ifndef _BIT_SET_H_
#define _BIT_SET_H_

#include <stdlib.h>

#define WORDSZ 64

typedef struct BitSet {
    int* bits;
    int numBits;
    int numWords;
} BitSet_t;

static inline BitSet_t* init_bitset(int numBits) {
    BitSet_t* temp = calloc(1, sizeof(BitSet_t));
    temp -> numBits = numBits;
    temp -> numWords = (numBits + WORDSZ - 1) / WORDSZ; // true ceil
    temp -> bits = calloc(temp -> numWords, sizeof(uint64_t));
    return temp;
}

static inline void set_bit(BitSet_t* bitset, int bit){
    bitset -> bits[bit >> 6] |= 1 << (bit & 63);
}

static inline void clear_bit(BitSet_t* bitset, int bit){
    bitset -> bits[bit/WORDSZ] &= ~(1 << (bit % WORDSZ));
}

static inline bool get_bit(BitSet_t* bitset, int bit){
    return (bitset -> bits[bit/WORDSZ] & (1 << (bit % WORDSZ))) != 0;
}

static inline int count_1s(BitSet_t* bitset){
    int sum = 0;
    for (int k = 0; k < bitset -> numWords; k++)
        sum += __builtin_popcountll ((uint64_t) bitset -> bits[k]);
    return sum;
}

static inline void destroy_bitset(BitSet_t* bitset) {
    if (bitset) {
        if (bitset -> bits)
            free(bitset -> bits);
        free(bitset);
    }
}

#endif