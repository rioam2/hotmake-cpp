#pragma once
#include <algorithm>
#include <fstream>
#include <functional>
#include <iterator>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

using std::function;
using std::map;
using std::string;
using std::vector;

/**
 * Represents a structured version of a Makefile. Allows
 * for ease of access to make rules, targets, dependencies, etc.
 */
class MakeRecord {
   public:
    /**
    * Builds a new record from the Makefile in the program's execution
    * working directory.
    */
    MakeRecord();
    /**
    * Representative structure for a single make rule. Contains
    * member variables for the rule's target as well its dependencies.
    */
    class Rule {
       public:
        Rule(){/* Nothing */};
        Rule(const string& t, const vector<string>& d)
            : target(t), deps(d){/* Nothing */};
        void addDeps(const vector<string>& other) {
            deps.insert(deps.end(), other.begin(), other.end());
        }
        void removeDep(const string& dep) {
            for (auto it = deps.begin(); it != deps.end(); it++) {
                if (*it == dep) {
                    deps.erase(it);
                    return;
                }
            }
        }

        /* Internal class storage: */
        string target;
        vector<string> deps;
    };

    /**
     * Retrieves a given rule from the Makefile.
     */
    MakeRecord::Rule getRule(const string& rule);

    /**
     * Checks if a rule exists in the current makefile instance
     */
    bool ruleExists(const string& rule);

   private:
    /**
     * Wrapper function for iterating over lines in
     * a valid UTF-formatted Makefile using UNIX fs API.
     * Throws an exception if no valid makefile is in
     * the execution working directory.
     */
    void __forEachLine(function<void(string)> cb);

    /* Internal class storage for make rules */
    map<string, Rule> __rules;
};