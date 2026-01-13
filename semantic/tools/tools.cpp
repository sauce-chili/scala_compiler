#include "tools.h"
#include <string>

vector<string> split(const string &str, char separator) {
    vector<string> strings;
    int startIndex = 0, endIndex = 0;
    for (int i = 0; i <= str.size(); i++) {

        // If we reached the end of the word or the end of the input.
        if (str[i] == separator || i == str.size()) {
            endIndex = i;
            string temp;
            temp.append(str, startIndex, endIndex - startIndex);
            if(temp.size())
            {
                strings.push_back(temp);
            }
            startIndex = endIndex + 1;
        }
    }
    return strings;
}

bool isStartWith(const string &str, const string &substr) {
    return str.find(substr) == 0;
}

string Exception::getMessage() {
    return message;
}

Exception::Exception(ExceptionType exceptionType, const string& message) {
    Exception::counter++;
    this->exceptionType = exceptionType;
    this->message = message;
}

Exception::Exception(Exception::ExceptionType exceptionType, const string& message, int line) {
    Exception::counter++;
    this->exceptionType = exceptionType;
    string tmp =  "line " + to_string(line) + ": ";
    if(!line) tmp = "";
    this->message = tmp + message;
}

string Modifiers::toString() {
    return accessModifierToString() + overrideModifierToString() + inheritModifierToString();
}

string Modifiers::accessModifierToString() {
    switch (this->accessModifier) {
        case AccessModifiers::public_:
            return "public ";
        case AccessModifiers::private_:
            return "private ";
        case AccessModifiers::protected_:
            return "protected ";
        default: return "";
    }
}

string Modifiers::overrideModifierToString() {
    switch (this->overrideModifier) {
        case OverrideModifiers::override_:
            return "override ";
        default: return "";
    }
}

string Modifiers::inheritModifierToString() {
    switch (this->inheritModifier) {
        case InheritModifiers::abstract_:
            return "abstract ";
        case InheritModifiers::final_:
            return "final ";
        case InheritModifiers::sealed_:
            return "sealed ";
        default: return "";
    }
}

bool Modifiers::isEquals(const Modifiers& other) {
    return other.accessModifier == this->accessModifier
        && other.inheritModifier == this->inheritModifier
        && other.overrideModifier == this->overrideModifier;
}

int Exception::counter = 0;