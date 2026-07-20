#!/usr/bin/env python

# File: __main__.py
# Date: 20 July 2026
# Author: T. Quinn Smith
# Purpose: Main function for SOAP.

import argparse
import sys
import os
from multiprocessing import Pool

def print_help() -> None:
    print("""
SOAP v1.0
-------------------------
Written by T. Quinn Smith
The Pennsylvania State University
Usage: soap [options] <step1.vcf.gz> <step2.vcf.gz> ...

Options:
    -k,--dimension         INT         Perform DMD in INT dimensions.
    -g,--garud             INT         Calculate Garud's H statistics on haplotypes of
                                            INT base pairs. Cannot be used with --ihs.
    -e,--ihs                           Calculate iHS at each position instead of allele frequency.
                                            Cannot be used with --garud.
    -u,--unphased                      Use unphased versions of Garud's H (G) or unphased iHS.
    -f,--fuzzy                         Use fuzzy clustering of haplotypes instead of random assignment
                                            when calculating Garud's H (G).
    -l,--long                          Output statistics for each input file.
    -t,--threads                       Number of threads to use in execution.
                                            Default 1.
    -m,--MAF               DOUBLE      Biallelic sites with MAF <= DOUBLE are dropped.
                                            Default 0; monomorphic sites are dropped.
    -n,--missingAF         DOUBLE      Sites with proportion of missing genotypes >= DOUBLE are dropped.
                                            Default 1; no sites with missing genotypes are dropped.
    -o,--out               STR         The output file basename.
                                            Default stdout.
    """)

def get_args():
    # Parse them all as strings and check them later.
    parser = argparse.ArgumentParser()
    parser.add_argument('-k', '--dimension', nargs=1, type=str)
    parser.add_argument('-g', '--garud', nargs=1, type=str)
    parser.add_argument('-e', '--ihs', type=bool)
    parser.add_argument('-u', '--unphased', type=bool)
    parser.add_argument('-f', '--fuzzy', type=bool)
    parser.add_argument('-l', '--long', type=bool)
    parser.add_argument('-t', '--threads', nargs=1, type=str)
    parser.add_argument('-m', '--MAF', type=str)
    parser.add_argument('-n', '--afMissing', nargs=1, type=str)
    parser.add_argument('-o', '--out', nargs=1, type=str)
    # The input files.
    parser.add_argument('steps', nargs="*")
    return parser.parse_args()

# Check args are valid.
def check_args(args) -> bool:
    try:
        # Check dimension.
        if int(args.k) <= 0:
            print("-k,--dimension must be > 0. Exiting ...")
            return False 
        
        # Check Garud.
        if int(args.garud) < 0:
            print("-g,--garud must be > 0. Exiting ...")
            return False 

        # Check MAF.
        if float(args.MAF) < 0 or float(args.MAF) >= 1:
            print("-m,--MAF must be in [0, 1). Exiting ...")
            return False 

        # Check Missing.
        if float(args.afMissing) < 0 or float(args.afMissing) > 1:
            print("-m,--MAF must be in [0, 1). Exiting ...")
            return False 

        # Check Number of threads.
        if int(args.threads) < 0:
            print("-t,--threads must be >= 1. Exiting ...")
            return False

        # Check step files exist.
        for filename in args.steps:
            if not os.path.isfile(filename):
                print("Step files do not exist! Exiting ...")
                return False
    
    # Catch any conversion errors.
    except ValueError as e:
        print("Invalid argument type:", e)
        return False

    return True

def main() -> None:

    # Print help.
    if len(sys.argv) == 1:
        print_help()
        return
    
    # Verify command line arguments.
    args = get_args()
    if not check_args(args):
        return

    # Process each file.
    if int(args.threads) == 1:
        steps = [calculate_stats((filename, args)) for filename in args.steps]
    else:
        with Pool(processes=int(args.threads)) as pool:
            steps = pool.map(calculate_stats, zip(args.steps, len(args.steps) * [args]))



if __name__ == "__main__":
    main()