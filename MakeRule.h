#pragma once
#include <fstream>
#include <functional>
#include <string>
#include <vector>

using std::function;
using std::ifstream;
using std::string;
using std::vector;

/**
 * Utilized to keep track of a make
 * rule and all of its dependencies
 */
class MakeRule {
   public:
    MakeRule() = delete; /* Force custom constructor */
    MakeRule(const MakeRule&) = delete;
    /**
     * Custom constructor for creating a new MakeRule
     * given the name of a make target rule. Internally
     * uses makefile in current working directory as src.
     */
    MakeRule(const string& rule);

   private:
    /**
     * Wrapper function for iterating over lines in
     * a valid UTF-formatted Makefile. Returns 0 upon 
     * success and 1 upon failure to locate/read a file
     * with a valid Makefile name.
     */
    void __forEachLine(function<void(string)> cb);

    /**
     * Internal helper function designed to build
     * the class variables with target and dependency
     * rule specifications.
     */
    void __buildDeps(const string& rule);

    /* Private internal variables */
    vector<string> __dependencies;
    string __target;
};