#include "ClangError.h"
#include <iostream>
#include <regex>

using std::cout;
using std::endl;
using std::regex;
using std::regex_match;
using std::smatch;
using std::string;

ClangError::ClangError(string stderr) {
    smatch errMatch;
    regex errReg("((?:.\\/)?\\w+(?:.\\w+)?):(\\d+):(\\d+):\\s([^\\\\n]+):\\s(.*)", regex::ECMAScript);
    /* Iterate over each error regex match in output */
    string::const_iterator pos(stderr.cbegin());
    while (regex_search(pos, stderr.cend(), errMatch, errReg)) {
        /* Update iterable */
        pos = errMatch.suffix().first;
        /* Push onto list of errors */
        ERR err(errMatch[1], errMatch[2], errMatch[3], errMatch[4], errMatch[5]);
        __errors.push_back(err);
    };
};

std::vector<ClangError::ERR> ClangError::getErrors() {
    return __errors;
}
