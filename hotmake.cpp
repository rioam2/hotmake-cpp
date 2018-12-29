#include <iostream>
#include <string>

using std::string;

/**
 * Prints the CLI usage to the terminal.
 */
void printUsage() {
    std::cout << "Usage: hotmake [make rule] [execution command]\n";
}

int main(int argc, char* argv[]) {
    /* Process user input */
    string rule;
    string exec;
    switch (argc) {
        case 1:
            printUsage();
            return 1;
        case 2:
            rule = argv[1];
            exec = "";
            break;
        default:
            rule = argv[1];
            exec = argv[2];
    }
    return 0;
}