#include <string>

#include "MakeRule.h"

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
    cout << "Usage: " << executable << " [make rule]\n";
    return 1;
}

int main(int argc, char* argv[]) {
    try { /* Primitive exception handling */
        /* Process user input */
        if (argc != 2) return printUsage(argv[0]);
        /* Make a new job out of target */
        MakeRule job(argv[1]);
    } catch (const char* except) {
        cerr << except << endl;
        return 1;
    }
    return 0;
}