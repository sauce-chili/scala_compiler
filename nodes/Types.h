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
    //INTEGER_LITERAL,
    //CHAR_LITERAL,
    //DOUBLE_LITERAL,
    //STRING_LITERAL,
    //BOOL_LITERAL,
    //IDENTIFIER,
    //NULL_LITERAL,
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


enum SimpleExprType {
    OBJECT_CREATING,
    BLOCK_STATS,
    SIMPLE_EXPR_1,
};

enum SimpleExpr1Type {
    INTEGER_LITERAL,
    CHAR_LITERAL,
    DOUBLE_LITERAL,
    STRING_LITERAL,
    BOOL_LITERAL,
    NULL_LITERAL,
    IDENTIFIER,
    SUPER_FIELD_ACCESS,
    THIS_FIELD_ACCESS,
    EXPRESSION_FIELD_ACCESS,
    ARGUMENT_CALL,
    EMPTY_CALL,
    METHOD_CALL,
};

enum StableIdType {
    STABLE_ID_IDENTIFIER,
    STABLE_ID_SUPER_FIELD_ACCESS,
    STABLE_ID_THIS_FIELD_ACCESS,
};

enum StatType {
    TEMPLATE,
    ENUM,
    VAR_DECL,
    VAL_DECL,
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
std::string simpleExpr1ToString(SimpleExpr1Type t);
std::string simpleExprToString(SimpleExprType t);
std::string stableIdTypeToString(StableIdType t);
std::string statTypeToString(StatType t);


std::string exprTypeToString(ExprType t);
std::string exprStatToString(StatType t);
std::string typeToString(Type t);


#endif //SCALA_LEXER_TYPES_H