//
// Created by Dmitry Smirnov on 13.01.2026.
//

#ifndef SCALA_LEXER_SCOPE_H
#define SCALA_LEXER_SCOPE_H
#include <optional>
#include <string>
#include <vector>

using namespace std;

enum class ScopeType {
    GLOBAL, // уровень файла
    CLASS,
    METHOD_ARGS,
    METHOD_BODY,
    BLOCK // между {...}, в т.ч if-else, for,do/while и тд
};

class Scope {
public:
    Scope *external;
    Scope *parent; // скоуп класса родителя
    std::string className; // если скоуп класса, хранит имя текущего класса
    ScopeType type;
};

/**
* VarTable - умеет искать переменную по имени + текущий scope(не лезет выше к parent, а когда external->type == class прекращает поиск)
* FieldTable - умеет искать только по имени
* MethodTable сначала ищет по локальным переменным, затем по аргументам с учетом скоупа, т.е снача в табилцу local, затем param
* ClassTable - сначала запускает поиск по методу, если не найдено, то currentScope->type будет CLASS, ищем в полях,
* если нет то переключаем currentScope = currentScope->parent, по имени класс currentScope->parent->className ищем в  его полях, не найдено снова лезем в родительский до тех пор пока он не nullptr,
* тогда возращаемся в класс откуда наичнали поиск и ищем в GLOBAL
 */

#endif //SCALA_LEXER_SCOPE_H
