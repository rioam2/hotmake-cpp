#pragma once
#include <string>
#include <vector>

using std::string;

/**
 * Wrapper for handling compiler results/errors
 * and for extracting useful data for errors.
 */
class ClangError {
   public:
    ClangError(){/* Nothing */};
    ClangError(string stderr);
    /**
     * Internal structure for keeping track of errors
     */
    class ERR {
       public:
        ERR(){/* Nothing */};
        /* Generalized assignment constructor */
        ERR(string _file, string _row, string _column,
            string _type, string _details)
            : file(_file), row(_row), column(_column), type(_type), details(_details){/* Nothing */};
        string file;
        string row;
        string column;
        string type;
        string details;
    };

    /**
     * Getter method for returning vector of errors.
     */
    std::vector<ERR> getErrors();

   private:
    /* Internal class storage */
    std::vector<ERR> __errors;
};

#include "ClangError.cpp"