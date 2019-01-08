#include <stdlib.h>
#include <unistd.h>
#include <array>
#include <cstdio>
#include <exception>
#include <future>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include "MakeRecord.h"
#include "inotify-cxx/inotify-cxx.h"

#define VERSION "1.0"

using std::async;
using std::cerr;
using std::cout;
using std::endl;
using std::exception;
using std::string;
using std::system;
using std::vector;

/**
 * Prints the CLI usage to the terminal and returns
 * error code of 1.
 */
int printUsage(char* executable) {
    cerr << "Usage: " << executable << " [make rule]\n";
    return 1;
}

/**
 * Helps print interactive lines to the terminal which overwrite each other.
 * Flushes cout and uses carrage returns.
 */
void printInteractive(string str) {
    /* Keep track of last string printed to display so can be overwritten */
    static string lastPrint = str;
    cout << "\r";
    /* Print over last interactive line */
    for (size_t i = 0; i < lastPrint.size(); i++) cout << " ";
    /* Print new line over existing one */
    cout << "\r" << str;
    std::flush(cout);
    lastPrint = str;
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
    if (!pipe) throw std::runtime_error("execution of target failed.");
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != NULL) {
        result += buffer.data();
    }
    return result;
}

int main(int argc, char* argv[]) {
    /* Primitive exception handling */
    try {
        MakeRecord makefile;
        auto [target, deps] = (argc >= 2) ? makefile.getRule(argv[1]) : makefile.getRule();
        /* Assemble executable command */
        const char* executable = string("./" + target).c_str();
        /* Watch all dependent files */
        Inotify notify;
        system("clear"); /* Clear terminal window */
        cout << "\033[1;37mHotmake v" << VERSION << ":\033[0m \033[4m" << target << "\033[0m" << endl;
        for (string dep : deps) {
            cout << "  » " << dep << endl;
            dep = "./" + dep;
            InotifyWatch watch(dep, IN_MODIFY);
            notify.Add(watch);
        }
        auto interactiveBuild = [&]() {
            bool buildFailed = false;
            bool buildComplete = false;
            auto build = async(std::launch::async, [&] {
                string result = exec("make");
                /* Primitive error checking - fix later */
                if (result.find("fail") != result.npos) {
                    buildFailed = true;
                }
                buildComplete = true;
            });
            auto printProgress = async(std::launch::async, [&] {
                string color_building = "\033[1;93m";
                string color_complete = "\033[1;32m";
                string color_failed = "\033[1;31m";
                string base = "Building changes";
                printInteractive(color_building + "  " + base);
                /* Spinner animation courtesy of cli-spinners on GitHub :) - thank you! */
                vector<string> spinner = {"⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"};
                unsigned frame = 0;
                while (!buildComplete) {
                    cout << "\r" << color_building << spinner[frame] << " " << base;
                    std::flush(cout);
                    frame = (++frame) % spinner.size();
                    usleep(70000);
                }
                if (buildFailed) {
                    printInteractive(color_failed + "✖ Build failed");
                } else {
                    printInteractive(color_complete + "✔ Build complete");
                }
            });
        };
        interactiveBuild(); /* Run build on startup */
        /* Watch for file updates and squash redundant triggers */
        while (notify.WaitForEvents() && notify.WaitForEvents()) {
            if (0 != notify.GetEventCount()) interactiveBuild();
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