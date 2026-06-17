// File: Interface.h
// Date: 19 June 2025
// Author: T. Quinn Smith
// Principal Investigator: Dr. Zachary A. Szpiech
// Purpose: Parser command line options.

#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// All the possible values that define a run of DSTAR.
typedef struct {

} SOAPConfig_t;

// Parse commnad line arguments.
SOAPConfig_t* init_soap_config(int argc, char* argv[]);

void destroy_soap_config(SOAPConfig_t* config);

void print_help();

#endif