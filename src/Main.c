
#include "Interface.h"
#include "VCFParser.h"

int main (int argc, char *argv[]) {

    // Print help if no arguments were given.
    if (argc == 1) {
        print_help();
        return 0;
    }

    // Get configuration from user and exit if error.
    SOAPConfig_t* config = init_soap_config(argc, argv);
    if (config == NULL)
        return -1;

    return 0;
}