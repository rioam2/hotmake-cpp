#include "MakeRule.h"
#include <algorithm>
#include <iterator>
#include <map>
#include <regex>
#include <sstream>

using std::back_inserter;
using std::copy;
using std::istream_iterator;
using std::istringstream;
using std::map;
using std::pair;
using std::regex;
using std::smatch;

/* Debug only */
#include <iostream>
using std::cout;
using std::endl;

MakeRule::MakeRule(const string& rule) {
    __buildDeps(rule);
};

void MakeRule::__forEachLine(function<void(string)> cb) {
    string line;
    string validNames[3] = {"Makefile", "makefile", "GNUmakefile"};
    /* Process first match from valid makefile names */
    for (string fileName : validNames) {
        /* Generate a dictionary for makefile vars */
        map<string, string> vars;
        /**
         * Lambda helper to replace variable names in the
         * input string with their expansions as defined in
         * the vars map.
         */
        function<string(const string&)> substituteVars = [&](const string& input) -> string {
            string newline = input;
            regex varReg("(\\$\\(\\S+\\))");
            for (auto [varName, varDef] : vars) {
                while (newline.find(varName) != string::npos) {
                    /* Replace all instances of var with def: */
                    size_t begin = newline.find(varName);
                    size_t len = varName.size();
                    newline.replace(begin, len, varDef);
                }
            }
            return newline;
        };
        /**
         * Lambda helper to add new makefile variables to
         * file dictionary recursively. Returns 0 if the line
         * is a variable declaration, and 1 if the line is not.
         */
        auto addVarDef = [&](const string& line) -> bool {
            /* Matches lines with variable declarations */
            regex varDeclReg("(\\S+)(?:\\s+)=(?:\\s)([^\\n]+)");
            smatch varMatch;
            bool lineDeclaresVar = std::regex_match(line, varMatch, varDeclReg);
            if (lineDeclaresVar) {
                string varName = varMatch[1], varDef = varMatch[2];
                vars["$(" + varName + ")"] = varDef;
                return 1;
            }
            return 0;
        };
        /* Open and read file */
        ifstream file(fileName);
        if (file.is_open()) {
            while (file.good()) {
                getline(file, line);
                addVarDef(line);
                line = substituteVars(line);
                cb(line);
            }
            file.close();
            /* Return control after successful read */
            return;
        }
    }
    /* Failed to find/open file with given names */
    throw "No Makefile in current working directory";
};

void MakeRule::__buildDeps(const string& rule) {
    string phonyTargets;
    bool targetLocated = false;
    /* Regex definitions */
    regex ruleReg("(\\S+)(?:[^\\S\\n]+)?:(?:[^\\S\\n]+)([^\\n]+)", regex::ECMAScript);
    /* Iterate over makefile lines */
    __forEachLine([&](string line) {
        /* Match general makerule definition structure */
        smatch ruleMatch;
        bool ruleSearch = std::regex_match(line, ruleMatch, ruleReg);
        if (ruleSearch) {
            string target = ruleMatch[1];
            string deps = ruleMatch[2];
            /* Only process target rule */
            bool isTargetRule = target == rule;
            bool isDependentRule =
                std::find(__dependencies.begin(),
                          __dependencies.end(), target) !=
                        __dependencies.end()
                    ? true
                    : false;
            /* Set phony targets */
            if (target == ".PHONY") phonyTargets = deps;
            /* Process rule that is either target or dependent */
            if (isTargetRule || isDependentRule) {
                targetLocated = true;
                if (isTargetRule) __target = target;
                istringstream iss(deps);
                copy(istream_iterator<string>(iss),
                     istream_iterator<string>(),
                     back_inserter(__dependencies));
            }
        }
    });
    if (!targetLocated) throw "Make target does not exist";
    /* Swap out phony target if needed */
    if (phonyTargets.find(__target) != string::npos) {
        __target = __dependencies[0];
        __dependencies.erase(__dependencies.begin());
    }
};