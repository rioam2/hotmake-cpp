#include "MakeRecord.h"

using std::back_inserter;
using std::find;
using std::ifstream;
using std::istream_iterator;
using std::istringstream;
using std::regex;
using std::remove;
using std::smatch;

/* Debug only */
#include <iostream>
using std::cout;
using std::endl;

MakeRecord::MakeRecord() {
    smatch ruleMatch;
    regex ruleReg("(\\S+)(?:[^\\S\\n]+)?:(?:[^\\S\\n]+)([^\\n]+)", regex::ECMAScript);
    /* Process Makefile in current execution directory */
    __forEachLine([&](string line) -> void {
        /* Check if current line is a rule declaration */
        if (std::regex_match(line, ruleMatch, ruleReg)) {
            string target = ruleMatch[1];
            string deps = ruleMatch[2];
            /* Transfer deps string to vector */
            istringstream iss(deps);
            vector<string> depsVec = {
                istream_iterator<string>{iss},
                istream_iterator<string>{}};
            __rules[target] = Rule(target, depsVec);
        }
    });
    /* Second pass for adding recursive dependencies */
    vector<string> phonyTargets = (ruleExists(".PHONY"))
                                      ? __rules[".PHONY"].deps
                                      : vector<string>();
    for (auto&& [target, rule] : __rules) {
        if (target != ".PHONY") {
            for (string dep : rule.deps) {
                /* Check if dependency is phony */
                auto phonySearch = find(phonyTargets.begin(), phonyTargets.end(), dep);
                if (phonySearch != phonyTargets.end()) {
                    rule.deps.erase(
                        std::remove(rule.deps.begin(), rule.deps.end(), dep),
                        rule.deps.end());
                }
                /* If the dependency has it's own rule, add deps */
                if (ruleExists(dep)) {
                    rule.addDeps(__rules[dep].deps);
                }
            }
        }
    }
}

void MakeRecord::__forEachLine(function<void(string)> cb) {
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
        string line;
        ifstream file(fileName);
        if (file.is_open()) {
            while (file.good()) {
                getline(file, line);
                addVarDef(line);
                cb(substituteVars(line));
            }
            file.close();
            /* Return control after successful read */
            return;
        }
    }
    /* Failed to find/open file with given names */
    throw "No Makefile in current working directory";
};

bool MakeRecord::ruleExists(const string& rule) {
    return __rules.find(rule) != __rules.end();
}

MakeRecord::Rule MakeRecord::getRule(const string& rule) {
    auto search = __rules.find(rule);
    if (search == __rules.end()) {
        throw "Make rule does not exist";
    } else {
        return search->second;
    }
};