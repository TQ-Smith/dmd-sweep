
// File: VCFParser.c
// Date: 13 May 2025
// Author: T. Quinn Smith
// Principal Investigator: Dr. Zachary A. Szpiech
// Purpose: Parse VCF files. Adapted from EGGS

#include "VCFParser.h"

InputStream_t* init_input_stream(char* source) {
    InputStream_t* inputStream = (InputStream_t*) calloc(1, sizeof(InputStream_t));
    inputStream -> file = gzopen(source, "r");
    inputStream -> fpIn = ks_init(inputStream -> file);
    inputStream -> buffer = (kstring_t*) calloc(1, sizeof(kstring_t));
    return inputStream;
}

void destroy_input_stream(InputStream_t* inputStream) {
    if (inputStream == NULL)
        return;
    if (inputStream -> fpIn != NULL)
        ks_destroy(inputStream -> fpIn);
    if (inputStream -> buffer != NULL) {
        if (inputStream -> buffer -> s != NULL)
            free(inputStream -> buffer -> s);
        free(inputStream -> buffer);
    }
    gzclose(inputStream -> file);
    free(inputStream);
}

VCF_t* init_vcf(InputStream_t* inputStream, bool eat) {
    if (inputStream == NULL)
        return NULL;

    // Allocate our VCF_t.
    VCF_t* vcf = (VCF_t*) calloc(1, sizeof(VCF_t));

    // If we have header lines to eat.
    if (eat) {
        int dret;
        do {
            ks_getuntil(inputStream -> fpIn, '\n', inputStream -> buffer, &dret);
        } while (strncmp(inputStream -> buffer -> s, "#C", 2) != 0);
    }

    // Count the number of samples in the VCF file.
    int numSamples = 0;
    for (int i = 0; i < inputStream -> buffer -> l; i++)
        if (inputStream -> buffer -> s[i] == '\t')
            numSamples++;
    vcf -> numSamples = numSamples - 8;
    // Copy the sample names from the header line.
    vcf -> sampleNames = calloc(vcf -> numSamples, sizeof(char*));
    // Read in the sample names.
    char* header = strdup(inputStream -> buffer -> s);
    char* tok = NULL;
    tok = strtok(header, "\t");
    for (int i = 0; i < 9; i++)
        tok = strtok(NULL, "\t");
    for (int i = 0; i < vcf -> numSamples; i++) {
        vcf -> sampleNames[i] = strdup(tok);
        tok = strtok(NULL, "\t");
    }
    
    return vcf;
}

bool get_next_vcf_record(Record_t* record, InputStream_t* inputStream, bool dropMonomorphicSites, double maf, double afMissing) {

    int dret = 0, numTabs = 0, prevIndex = 0, numAlleles = 2;

    while (true) {
        // Get the next line.
        ks_getuntil(inputStream -> fpIn, '\n', inputStream -> buffer, &dret);

        // If end of file, return false.
        if (ks_eof(inputStream -> fpIn) || inputStream -> buffer -> l == 0)
            return false;

        // This is alittle clunky, but I think it is faster than splitting on '\t'.
        for (int i = 0; i <= inputStream -> buffer -> l; i++) {
            // If end of the line is reached or a tab was encountered.
            if (i == inputStream -> buffer -> l || inputStream -> buffer -> s[i] == '\t') {
                if (numTabs == 0) {
                    // The first field in a record is the chromosome name.
                    if (record -> chrom != NULL)
                        free(record -> chrom);
                    record -> chrom = strndup(inputStream -> buffer -> s, i);
                } else if (numTabs == 1) {
                    // The second field is the position on the chromosome.
                    record -> position = (int) strtol(inputStream -> buffer -> s + prevIndex + 1, (char**) NULL, 10);
                } else if (numTabs == 4) {
                    // The fifth field holds the ALT alleles. Each record has at least two alleles.
                    //  Each additional allele is appended withinputStream -> buffer -> s a ','. For each ',' encountered,
                    //  increment the number of alleles in the record.
                    for (int j = prevIndex + 1; inputStream -> buffer -> s[j] != '\t'; j++)
                        if (inputStream -> buffer -> s[j] == ',')
                            numAlleles++;
                } else if (numTabs > 8) {
                    int sampleIndex = numTabs - 9;
                    set_bit(record -> leftMissing, sampleIndex);
                    set_bit(record -> rightMissing, sampleIndex);
                    char* start = inputStream -> buffer -> s + prevIndex + 1;
                    char* next = start;
                    if (start[0] != '.')
                        set_bit(record -> left, sampleIndex);
                    if ((next[0] == '|' || next[0] == '/') && next[1] != '.')
                        set_bit(record -> right, sampleIndex);
                }
                prevIndex = i;
                numTabs++;
            }
        }
        record -> numAlleles = numAlleles;

        // Only consider biallelic sites.
        if (numAlleles != 2)
            continue;

        int numAlts = count_1s(record -> left) +  count_1s(record -> right);
        int numMissing = count_1s(record -> leftMissing) +  count_1s(record -> rightMissing);

        // Drop site if monomorphic.
        if (dropMonomorphicSites && numMissing == 0 && (numAlts == record -> numSamples || numAlts == 0))
            continue;

        // MAF and missing threshold.
        if (numAlleles / ((double) 2 * record -> numSamples) < maf)
            continue;
        if (numMissing / ((double) 2 * record -> numSamples) < afMissing)
            continue;

        break;
    }

    // Successfully parsed record.
    return true;
}

void parse_vcf(VCF_t* vcf, InputStream_t* inputStream, bool dropMonomorphicSites, double maf, double afMissing) {
    int recordIndex = 0;
    while (true) {
        // Allocate memory for a new record.
        Record_t* record = (Record_t*) calloc(1, sizeof(Record_t));
        record -> left = init_bitset(vcf -> numSamples);
        record -> right = init_bitset(vcf -> numSamples);
        record -> leftMissing = init_bitset(vcf -> numSamples);
        record -> rightMissing = init_bitset(vcf -> numSamples);
        record -> numSamples = vcf -> numSamples;
        record -> recordIndex = recordIndex;

        // While not EOF, add record to the list.
        if (get_next_vcf_record(record, inputStream, dropMonomorphicSites, maf, afMissing)) {
            if (vcf -> numRecords == 0) {
                vcf -> headRecord = record;
                vcf -> tailRecord = record;
            } else {
                record -> prevRecord = vcf -> tailRecord;
                vcf -> tailRecord -> nextRecord = record;
                vcf -> tailRecord = record;
            }
            vcf -> numRecords++;
        } else {
            destroy_record(record);
            break;
        }
        recordIndex++;
    }

}

void destroy_record(Record_t* record) {
    if (record == NULL)
        return;
    if (record -> left != NULL)
        destroy_bitset(record -> left);
    if (record -> right != NULL)
        destroy_bitset(record -> right);
    if (record -> leftMissing != NULL)
        destroy_bitset(record -> leftMissing);
    if (record -> rightMissing != NULL)
        destroy_bitset(record -> rightMissing);
    if (record -> chrom != NULL)
        free(record -> chrom);
    free(record);
}

void destroy_vcf(VCF_t* vcf) {
    if (vcf == NULL)
        return;
    Record_t* temp = NULL;
    for (int i = 0; i < vcf -> numRecords; i++) {
        temp = vcf -> headRecord;
        vcf -> headRecord = vcf -> headRecord -> nextRecord;
        destroy_record(temp);
    }
    if (vcf -> sampleNames != NULL) {
        for (int i = 0; i < vcf -> numSamples; i++) {
            free(vcf -> sampleNames[i]);       
        }
        free(vcf -> sampleNames);
    }
    free(vcf);
}