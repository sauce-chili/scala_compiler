//
// Created by Dmitry Smirnov on 13.01.2026.
//

#ifndef SCALA_LEXER_SCOPE_H
#define SCALA_LEXER_SCOPE_H

enum class ScopeType {
    // мб сюда что-то добавиться
    BLOCK, // между {...}, в т.ч if-else,do/while и тд
    FOR, // для генераторов, тк там тоже определяются переменные
};

class Scope {
    static unsigned int scopeSeqId;

public:
    const unsigned int scopeId; // мб может понадобиться в мапах
    Scope *external;
    ScopeType type;

    Scope(Scope *external, ScopeType type) : scopeId(++scopeSeqId), external(external), type(type) {
    }

    bool operator==(const Scope &) const;
};

inline bool Scope::operator==(const Scope &other) const {
    return scopeId == other.scopeId &&
           external == other.external &&
           type == other.type;
}

#endif //SCALA_LEXER_SCOPE_H
