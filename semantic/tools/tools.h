#ifndef COMPILER_TOOLS_H
#define COMPILER_TOOLS_H
#define varName(x) #x
#define all(x) x.begin(), x.end()
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

vector<string> split(const string &str, char separator);

class Exception{
    static vector<Exception> exceptions;

private:
    string message;
    int line;
public:
    enum ExceptionType {
        UNEXPECTED,
        TYPE_ERROR,
    };

    static int counter;
    ExceptionType exceptionType;
    string getMessage();
    Exception(ExceptionType exceptionType, const string& message);
    Exception(ExceptionType exceptionType, const string& message, int line);

};

#endif //COMPILER_TOOLS_H

