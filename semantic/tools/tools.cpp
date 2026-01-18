#include "tools.h"
#include <string>
#include <set>

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

string Modifiers::toString() {
    string str;

    if (modifiers) {
        for (ModifierType mt: *modifiers) {
            if (mt) {
                str += modifierToString(mt) + "\n";
            }
        }
    }

    return str;
}

bool Modifiers::isEquals(const Modifiers &other) {
    if (modifiers == nullptr || other.modifiers == nullptr) {
        return modifiers == other.modifiers;
    }

    if (modifiers->size() != other.modifiers->size()) {
        return false;
    }

    std::multiset<ModifierType> a(modifiers->begin(), modifiers->end());
    std::multiset<ModifierType> b(other.modifiers->begin(), other.modifiers->end());

    return a == b;
}

