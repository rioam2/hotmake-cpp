#include <string>

#include "MakeRecord.h"

#include <iostream>
using std::cerr;
using std::cout;
using std::endl;
using std::string;

/**
 * Prints the CLI usage to the terminal and returns
 * error code of 1.
 */
int printUsage(char* executable) {
    cerr << "Usage: " << executable << " [make rule]\n";
    return 1;
}

int main(int argc, char* argv[]) {
    if (argc != 2) return printUsage(argv[0]);
    /* Primitive exception handling */
    try {
        MakeRecord makefile;
        auto [target, deps] = makefile.getRule(argv[1]);
    } catch (const char* e) {
        cerr << e << endl;
        return 1;
    }
    return 0;
}