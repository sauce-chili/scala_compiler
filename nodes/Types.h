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

    OPERATOR,
    FUNCTION_CALL,
    BRACKETS,
    ASSIGNMENT,
    THIS,
    SUPER,
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

enum PrefixExprType {
    UNARY_PLUS,
    UNARY_MINUS,
    NOT,
    BIT_NOT,
};

enum StableIdType {
    STABLE_ID_IDENTIFIER,
    STABLE_ID_SUPER_FIELD_ACCESS,
    STABLE_ID_THIS_FIELD_ACCESS,
};

enum StatType {
    TEMPLATE,
    CLASS,
    OBJECT,
    TRAIT,
    ENUM,
    VAR_DECL,
    VAL_DECL,
    FUN_DEF,
};

enum ClassParamType {
    VAL_CLASS_PARAM,
    VAR_CLASS_PARAM,
    UNMARKED_CLASS_PARAM,
};

enum EnumCaseType {
    CASE_WITH_EXTENDS,
    CASE_WITH_PARAMS,
    CASE_WITH_IDS,
};

enum DefinitionType {
    VAR_DEFS,
    FUN_DEFINITION,
    TEMPLATE_DEFINITION,
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
std::string prefixExprToString(PrefixExprType t);
std::string classParamTypeToString(ClassParamType t);
std::string classEnumCaseTypeToString(EnumCaseType t);
std::string definitionTypeToString(DefinitionType t);


std::string exprTypeToString(ExprType t);
std::string exprStatToString(StatType t);
std::string typeToString(Type t);


#endif //SCALA_LEXER_TYPES_H