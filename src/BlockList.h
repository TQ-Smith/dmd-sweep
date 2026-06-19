
#ifndef _BLOCK_LIST_H_
#define _BLOCK_LIST_H_

#include <stdbool.h>

typedef struct Block {
    char* chrom;
    int startPosition;
    int endPosition;
    int numLoci;

    double freq;
    double ihs;
    double h1;
    double h2;
    double h12;

    struct Block* next;
} Block_t;

typedef struct BlockList {
    int numRecords;
    bool phased;
    bool fuzzy;
    Block_t* head;
    Block_t* tail;
} BlockList_t;

BlockList_t* init_block_list(bool phased, bool fuzzy);

Block_t* init_block(char* chrom, int startPosition, int endPosition);

void append_block(BlockList_t* blockList, Block_t* block);

void destroy_block(BlockList_t* block);

void destroy_stats(BlockList_t* blockList);

#endif