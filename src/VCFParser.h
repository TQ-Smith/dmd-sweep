
// File: VCFParser.h
// Date: 13 May 2025
// Author: T. Quinn Smith
// Principal Investigator: Dr. Zachary A. Szpiech
// Purpose: Adapted from EGGS.

#ifndef _VCF_PARSER_H
#define _VCF_PARSER_H

#include <stdbool.h>
#include <zlib.h>
#include <stdio.h>
#include "kseq.h"
#include "BitSet.h"

// We use -1 to denote a missing genotype.
#define MISSING -1

// A stream to read GZIP files.
#define BUFFER_SIZE 4096
KSTREAM_INIT(gzFile, gzread, BUFFER_SIZE)

// Represents a record/locus.
typedef struct Record {
    // The chromosome.
    char* chrom;
    // The position on the chromosome.
    int position;
    // The total number of alleles at the locus.
    int numAlleles;
    // Redunant but useful. The number of samples in the genotypes array.
    int numSamples;
    // The index of the record in the list.
    int recordIndex;
    // Genotype information.
    BitSet_t* left;
    BitSet_t* right;
    BitSet_t* leftMissing;
    BitSet_t* rightMissing;
    // Record_t is a node in a doubly linked list.
    struct Record* nextRecord;
    struct Record* prevRecord;
} Record_t;


// A VCF is a linked list of records.
typedef struct {
    int numSamples;
    // Number of records in the list.
    int numRecords;
    // The sample names.
    char** sampleNames;
    // Head and tail pointers.
    Record_t* headRecord;
    Record_t* tailRecord;
} VCF_t;

// Packages all the necessary information to read from a GZIP file.
typedef struct {
    // The file we are reading.
    gzFile file;
    // The stream that wraps the file.
    kstream_t* fpIn;
    // The buffer to read in lines of the file.
    kstring_t* buffer;
} InputStream_t;

// From a file name, open up a GZIP stream.
// Accepts:
//  char* source -> The file name open. If null, read from stdin.
// Returns: InputStream_t*, the input stream we are reading from. NULL, if source does not exist. 
InputStream_t* init_input_stream(char* source);

// Closes file stream and deallocates memory.
void destroy_input_stream(InputStream_t* inputStream);

// From an input stream reading a VCF file, create an empty VCF.
// Accepts:
//  InputStream_t* inputStream -> The input stream to read from.
//  bool eat -> If we should eat the header lines.
// Returns: VCF_t*, an empty replicate with sampleNames set from VCF header info.
VCF_t* init_vcf(InputStream_t* inputStream, bool eat);

// Read in next VCF record from input stream.
// Accepts:
//  Record_t* record -> An allocated record. Set fields.
//  InputStream_t* inputStream -> The VCF file we are reading.
//  bool dropMonomorphicSites -> Drop sites if monomorphic.
//  double maf -> Minor allele frequency threshold.
//  double afMissing -> Missing allele frequency threshold.
// Returns: bool, true is a record was read. false, if eof.
bool get_next_vcf_record(Record_t* record, InputStream_t* inputStream, bool dropMonomorphicSites, double maf, double afMissing);

// Reads in whole VCF file and adds each record to VCF.
// Accepts:
//  VCF_t* vcf -> An allocated, but empty, vcf to add records to.
//  InputStream_t* inputStream -> The VCF file we are reading from.
//  bool dropMonomorphicSites -> Drop sites if monomorphic.
//  double maf -> Minor allele frequency threshold.
//  double afMissing -> Missing allele frequency threshold.
// Returns: void.
void parse_vcf(VCF_t* vcf, InputStream_t* inputStream, bool dropMonomorphicSites, double maf, double afMissing);

// Frees memory associated with a record.
void destroy_record(Record_t* record);

// Frees memory associated with a VCF.
void destroy_vcf(VCF_t* vcf);

#endif