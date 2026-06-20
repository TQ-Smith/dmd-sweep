
#include "Stats.h"
#include "kvec.h"
#include "khash.h"
#include <string.h>

void update_ehh() {

}

void calc_ehh(Record_t* record, bool upstream, bool phased, double* ihh2, double* ihh0, double* cihh2, double cihh0) {

}

void calc_ihh(Record_t* record, bool phased, double* ihh1, double* ihh0) {

}

BlockList_t* calc_ihs(VCF_t* vcf, bool phased) {
    BlockList_t* list = init_block_list(phased, false);
    Record_t* temp = vcf -> headRecord;
    for (int i = 0; i < vcf -> numRecords; i++) {
        Block_t* block = init_block(temp -> chrom, temp -> position, -1);
        double ihh1, ihh0;
        calc_ihh(temp, phased, &ihh1, &ihh0);
        block -> freq = (count_1s(temp -> left) + count_1s(temp -> right)) / (2.0 * vcf -> numSamples);
        block -> ihs = ihh1 > ihh0 ? ihh1 : 0-ihh0;
        temp = temp -> nextRecord;
    }
    return list;
}