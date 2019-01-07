#include <stdlib.h>
#include <array>
#include <cstdio>
#include <exception>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include "MakeRecord.h"
#include "inotify-cxx/inotify-cxx.h"

using std::cerr;
using std::cout;
using std::endl;
using std::exception;
using std::string;
using std::system;

/**
 * Prints the CLI usage to the terminal and returns
 * error code of 1.
 */
int printUsage(char* executable) {
    cerr << "Usage: " << executable << " [make rule]\n";
    return 1;
}

/**
 * POSIX execution wrapper to enable stdout capture.
 * Originally Posted on StackOverflow: 
 * https://stackoverflow.com/questions/478898/how-do-i-execute-a-command-and-get-output-of-command-within-c-using-posix
 */
std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

int main(int argc, char* argv[]) {
    /* Primitive exception handling */
    try {
        MakeRecord makefile;
        auto [target, deps] =
            argc >= 2 ? makefile.getRule(argv[1]) : makefile.getRule();
        /* Assemble executable command */
        const char* executable = string("./" + target).c_str();
        /* Watch all dependent files */
        Inotify notify;
        cout << "Watching source files: " << endl;
        for (string dep : deps) {
            dep = "./" + dep;
            cout << "\033[1;32m + " << dep << "\033[0m\n";
            InotifyWatch watch(dep, IN_MODIFY);
            notify.Add(watch);
        }
        while (1) {
            /* Squash redundant fs triggers */
            notify.WaitForEvents() && notify.WaitForEvents();
            if (0 != notify.GetEventCount()) {
                cout << exec("make");
            }
        }
    } catch (const char* e) {
        cerr << e << endl;
        return 1;
    } catch (InotifyException& e) {
        cerr << "Inotify exception occurred: " << e.GetMessage() << endl;
        return 1;
    } catch (exception& e) {
        cerr << "STL exception occurred: " << e.what() << endl;
        return 1;
    } catch (...) {
        cerr << "unknown exception occurred" << endl;
        return 1;
    }
    return 0;
}