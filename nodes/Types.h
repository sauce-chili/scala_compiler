#ifndef SCALA_LEXER_TYPES_H
#define SCALA_LEXER_TYPES_H

#include "string"

enum ModifierType {
    PRIVATE,
    PROTECTED,
    OPEN,
    FINAL,
    OVERRIDE,
};

enum ExprType {
    INTEGER_LITERAL,
    CHAR_LITERAL,
    DOUBLE_LITERAL,
    STRING_LITERAL,
    BOOL_LITERAL,
    IDENTIFIER,
    NULL_LITERAL,
    OPERATOR,
    FUNCTION_CALL,
    BRACKETS,
    ASSIGNMENT,
    THIS,
    SUPER,
    UNARY_PLUS,
    UNARY_MINUS,
    NOT,
    OR,
    AND,
    FUNC_CALL,
    FIELD_ACCESS,
    FUNC_ACCESS,
    ARRAY_EXPR,
    ARRAY_ACCESS,
    FOR,
    WHILE,
    DO_WHILE,
    IF,
};

enum StatType {
    TEMPLATE,
    ENUM,
};

enum Type {
    INT,
    CHAR,
    DOUBLE,
    STRING,
    BOOLEAN,
    ID,
    ARRAY,
    UNDEFINED,
    UNIT
};


std::string modifierToString(ModifierType m);
std::string exprTypeToString(ExprType t);
std::string exprStatToString(StatType t);
std::string typeToString(Type t);


#endif //SCALA_LEXER_TYPES_H