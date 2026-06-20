
#include <stdbool.h>
#include <math.h>
#include "VCFParser.h"
#include "BlockList.h"

BlockList_t* calc_ihs(VCF_t* vcf, bool phased);