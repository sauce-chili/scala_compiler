#ifndef COMPILER_TOOLS_H
#define COMPILER_TOOLS_H
#define varName(x) #x
#define all(x) x.begin(), x.end()

#include <iostream>
#include <vector>
#include <algorithm>
#include <list>
#include "nodes/Types.h"

using namespace std;

vector<string> split(const string &str, char separator);

[[maybe_unused]] bool isStartWith(const string &str, const string &substr);

class Modifiers {
public:

public:
    string toString();

    bool isEquals(const Modifiers &other);

    std::list<ModifierType> *modifiers;
};

#endif //COMPILER_TOOLS_H
