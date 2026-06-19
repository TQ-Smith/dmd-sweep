
#include "Interface.h"
#include "kstring.h"
#include "ketopt.h"
#include <unistd.h>

void print_help() {
    fprintf(stderr, "\n");
    fprintf(stderr, "SOAP v1.0\n");
    fprintf(stderr, "----------------------\n\n");
    fprintf(stderr, "Written by T. Quinn Smith\n");
    fprintf(stderr, "Principal Investigator: Zachary A. Szpiech\n");
    fprintf(stderr, "The Pennsylvania State University\n\n");
    fprintf(stderr, "Usage: soap [options] <step1.vcf.gz> <step2.vcf.gz> ...\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "    -k,--dimension         INT         Perform DMD in INT dimensions.\n");
    fprintf(stderr, "    -g,--garud             INT1,INT2   Calculate Garud's H statistics in sliding widow of width INT1\n");
    fprintf(stderr, "                                           and step INT2. Cannot be used with --ihs.\n");
    fprintf(stderr, "    -e,--ihs                           Calculate iHS at each position instead of allele frequency.\n");
    fprintf(stderr, "                                           Cannot be used with --garud.\n");
    fprintf(stderr, "    -u,--unphased                      Use unphased versions of Garud's H (G) or unphased iHS.\n");
    fprintf(stderr, "    -f,--fuzzy                         Use fuzzy clustering of haplotypes instead of random assignment\n");
    fprintf(stderr, "                                           when calculating Garud's H.\n");
    fprintf(stderr, "    -l,--long                          Output statistics for each input file.\n");
    fprintf(stderr, "    -m,--MAF               DOUBLE      Biallelic sites with MAF <= DOUBLE are dropped.\n");
    fprintf(stderr, "                                           Default 0; monomorphic sites are dropped.\n");
    fprintf(stderr, "    -n,--missingAF         DOUBLE      Sites with proportion of missing genotypes >= DOUBLE are dropped.\n");
    fprintf(stderr, "                                           Default 1.\n");
    fprintf(stderr, "    -o,--out               STR         The output file basename.\n");
    fprintf(stderr, "                                           Default stdout.\n");
    fprintf(stderr, "\n");
}

// Our options.
static ko_longopt_t long_options[] = {
    {"dimension",       ko_required_argument,         'k'},
    {"garud",           ko_required_argument,         'g'},
    {"ihs",             ko_no_argument,               'e'},
    {"unphased",        ko_no_argument,               'u'},
    {"fuzzy",           ko_no_argument,               'f'},
    {"long",            ko_no_argument,               'l'},
    {"MAF",             ko_required_argument,         'm'},
    {"missingAF",       ko_required_argument,         'n'},
    {"out",             ko_required_argument,         'o'},
    {0, 0, 0}
};

int check_configuration(SOAPConfig_t* config) {
    return 0;
}

SOAPConfig_t* init_soap_config(int argc, char* argv[]) {
    return NULL;
}

void destroy_soap_config(SOAPConfig_t* config) {

}