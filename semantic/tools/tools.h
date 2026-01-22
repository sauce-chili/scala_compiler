#ifndef COMPILER_TOOLS_H
#define COMPILER_TOOLS_H
#define varName(x) #x
#define all(x) x.begin(), x.end()

#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdint>

#include "nodes/Types.h"

class ModifiersNode;
using namespace std;

vector<string> split(const string &str, char separator);

[[maybe_unused]] bool isStartWith(const string &str, const string &substr);

class Modifiers {
public:
    vector<ModifierType> modifiers;

    string toString();

    bool isEquals(const Modifiers &other);

    vector<ModifierType> accessModifiers();

    vector<ModifierType> overrideModifiers();

    vector<ModifierType> inheritModifiers();

    static bool isAccessModifier(ModifierType type);

    static bool isOverrideModifier(ModifierType type);

    static bool isInheritModifier(ModifierType type);

    static Modifiers createFromModifiersNode(const ModifiersNode &modifiersNode);

    Modifiers(vector<ModifierType> modifiers) : modifiers(modifiers) {
    };

    bool hasModifier(ModifierType type) const ;
};

class Exception {
    static vector<Exception> exceptions;

    string message;
    int line;

public:
    enum ExceptionType {
        UNEXPECTED,
        TYPE_ERROR,
        NOT_EXIST,
    };

    static int counter;
    ExceptionType exceptionType;

    string getMessage();

    Exception(ExceptionType exceptionType, const string &message);

    Exception(ExceptionType exceptionType, const string &message, int line);
};

typedef std::uint8_t byte_t;

struct bytearray_t {
    size_t length;
    byte_t *bytes;

    bytearray_t() {
        length = 0;
        bytes = nullptr;
    }

    bytearray_t(size_t length, byte_t *content) : length(length), bytes(content) {
    }

    void reverseByteOrder();
};

#endif //COMPILER_TOOLS_H
