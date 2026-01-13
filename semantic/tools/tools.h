#ifndef COMPILER_TOOLS_H
#define COMPILER_TOOLS_H
#define varName(x) #x
#define all(x) x.begin(), x.end()
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

vector<string> split(const string &str, char separator);

[[maybe_unused]] bool isStartWith(const string &str, const string& substr);

class Modifiers {
public:
    enum class AccessModifiers {
        private_,
        public_,
        protected_,
        access_modifier_not_set,
    };

    enum class OverrideModifiers {
        override_,
        override_modifier_not_set,
    };

    enum class InheritModifiers {
        abstract_,
        final_,
        sealed_,
        inherit_modifier_not_set,
    };

public:
    string toString();
    string accessModifierToString();
    string overrideModifierToString();
    string inheritModifierToString();
    bool isEquals(const Modifiers& other);

    AccessModifiers accessModifier;
    OverrideModifiers overrideModifier;
    InheritModifiers inheritModifier;
};

class Exception{
    static vector<Exception> exceptions;

private:
    string message;
    int line;
    Modifiers modifiers;
public:
    enum ExceptionType {
        UNEXPECTED,
        TYPE_ERROR,
        NOT_EXIST,
    };

    static int counter;
    ExceptionType exceptionType;
    string getMessage();
    Exception(ExceptionType exceptionType, const string& message);
    Exception(ExceptionType exceptionType, const string& message, int line);

};

#endif //COMPILER_TOOLS_H

