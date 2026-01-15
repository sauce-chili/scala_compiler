#ifndef SCALA_LEXER_TYPES_H
#define SCALA_LEXER_TYPES_H

#include "string"

enum ModifierType {
    _PRIVATE = 1,
    _PROTECTED = 2,
    _ABSTRACT = 3,
    _FINAL = 4,
    _SEALED = 5,
    _OVERRIDE = 6,
};

enum ExprType {
    _IF_ELSE = 7,
    _IF = 8,
    _WHILE = 9,
    _TRY = 10,
    _DO_WHILE = 11,
    _THROW = 12,
    _RETURN_EMPTY = 13,
    _RETURN_EXPR = 14,
    _FOR_WITHOUT_YIELD = 15,
    _FOR_WITH_YIELD = 16,
    _INFIX = 17,
    _ASSIGNMENT = 18,
};


enum SimpleExprType {
    _INSTANCE_CREATING = 19,
    _BLOCK_STATS = 20,
    _SIMPLE_EXPR_1 = 21,
    _ARRAY_CREATING = 22,
};

enum SimpleExpr1Type {
    _INTEGER_LITERAL = 23,
    _CHAR_LITERAL = 24,
    _DOUBLE_LITERAL = 25,
    _STRING_LITERAL = 26,
    _BOOL_LITERAL = 27,
    _NULL_LITERAL = 28,
    _IDENTIFIER = 29,
    _SUPER_FIELD_ACCESS = 30,
    _THIS_FIELD_ACCESS = 31,
    _EXPRESSION_FIELD_ACCESS = 32,
    _ARGUMENT_CALL = 33,
    _EMPTY_CALL = 34,
    _METHOD_CALL = 35,
    _PLAIN_THIS = 36,
    _ARRAY_BUILDER = 37,
    _ARRAY_WITH_TYPE_BUILDER = 38,

};

enum PrefixExprType {
    _UNARY_PLUS = 39,
    _UNARY_MINUS = 40,
    _NOT = 41,
    _BIT_NOT = 42,
    _NO_UNARY_OPERATOR = 43,
};

enum StableIdType {
    _STABLE_ID_IDENTIFIER = 44,
    _STABLE_ID_SUPER_FIELD_ACCESS = 45,
    _STABLE_ID_THIS_FIELD_ACCESS = 46,
    _RECURSIVE_STABLE_ID = 47,
};

enum StatType {
    _TEMPLATE = 48,
    _CLASS = 49,
    _OBJECT = 50,
    _TRAIT = 51,
    _ENUM = 52,
    _VAR_DECL = 53,
    _VAL_DECL = 54,
    _FUN_DEF = 55,
};

enum ClassParamType {
    _VAL_CLASS_PARAM = 56,
    _VAR_CLASS_PARAM = 57,
    _UNMARKED_CLASS_PARAM = 58,
};

enum EnumCaseType {
    _CASE_WITH_EXTENDS = 59,
    _CASE_WITH_PARAMS = 60,
    _CASE_WITH_IDS = 61,
};

enum DefinitionType {
    _VAR_DEFS = 62,
    _FUN_DEFINITION = 63,
    _TEMPLATE_DEFINITION = 64,
};

enum Type {
    _INT,
    _CHAR,
    _DOUBLE,
    _STRING,
    _BOOLEAN,
    _ID,
    _ARRAY,
    _UNDEFINED,
    _UNIT
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


#endif //SCALA_LEXER_TYPES_H
