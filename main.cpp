#include <iostream>

/**
 * Prints the CLI usage to the terminal.
 */
void printUsage() {
    std::cout << "Usage: hotmake [make rule] [execution command]\n";
}

int main(int argc, char* argv[]) {
    printUsage();
    return 0;
}