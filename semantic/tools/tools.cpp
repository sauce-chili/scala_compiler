#include "tools.h"
#include <string>
#include <set>

#include "nodes/modifier/ModifiersNode.h"

vector<string> split(const string &str, char separator) {
    vector<string> strings;
    int startIndex = 0, endIndex = 0;
    for (int i = 0; i <= str.size(); i++) {
        // If we reached the end of the word or the end of the input.
        if (str[i] == separator || i == str.size()) {
            endIndex = i;
            string temp;
            temp.append(str, startIndex, endIndex - startIndex);
            if (temp.size()) {
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

    for (ModifierType mt: modifiers) {
        if (mt) {
            str += modifierToString(mt) + "\n";
        }
    }

    return str;
}

bool Modifiers::isEquals(const Modifiers &other) {

    if (modifiers.size() != other.modifiers.size()) {
        return false;
    }

    std::multiset<ModifierType> a(modifiers.begin(), modifiers.end());
    std::multiset<ModifierType> b(other.modifiers.begin(), other.modifiers.end());

    return a == b;
}

Modifiers Modifiers::createFromModifiersNode(const ModifiersNode &modifiersNode) {
    if (&modifiersNode == nullptr) return Modifiers({});
    vector<ModifierType> modifiers;
    for (const auto m: *modifiersNode.modifiers) {
        modifiers.push_back(m->type);
    }
    return Modifiers(modifiers);
}

bool Modifiers::hasModifier(ModifierType type) const {
    for (auto m : modifiers) {
        if (m == type) return true;
    }
    return false;
}


bool Modifiers::isAccessModifier(ModifierType type) {
    switch (type) {
        case ModifierType::_PRIVATE:
        case ModifierType::_PROTECTED:
            return true;
        default:
            return false;
    }
}

bool Modifiers::isOverrideModifier(ModifierType type) {
    return type == ModifierType::_OVERRIDE;
}

bool Modifiers::isInheritModifier(ModifierType type) {
    switch (type) {
        case ModifierType::_ABSTRACT:
        case ModifierType::_FINAL:
        case ModifierType::_SEALED:
            return true;
        default:
            return false;
    }
}

vector<ModifierType> Modifiers::accessModifiers() {
    vector<ModifierType> result;
    for (auto m: modifiers)
        if (isAccessModifier(m))
            result.push_back(m);
    return result;
}

vector<ModifierType> Modifiers::overrideModifiers() {
    vector<ModifierType> result;
    for (auto m: modifiers)
        if (isOverrideModifier(m))
            result.push_back(m);
    return result;
}

vector<ModifierType> Modifiers::inheritModifiers() {
    vector<ModifierType> result;
    for (auto m: modifiers)
        if (isInheritModifier(m))
            result.push_back(m);
    return result;
}


int Exception::counter = 0;

void bytearray_t::reverseByteOrder() {
    size_t left = 0;
    size_t right = length - 1;

    while (left < right) {
        std::swap(bytes[left], bytes[right]);
        left++;
        right--;
    }
}
