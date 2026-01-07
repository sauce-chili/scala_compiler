#ifndef SCALA_LEXER_TYPES_H
#define SCALA_LEXER_TYPES_H

#include "string"

enum ModifierType {
    _PRIVATE,
    _PROTECTED,
    _ABSTRACT,
    _FINAL,
    _SEALED,
    _OVERRIDE,
};

enum ExprType {
    _IF_ELSE,
    _IF,
    _WHILE,
    _TRY,
    _DO_WHILE,
    _THROW,
    _RETURN_EMPTY,
    _RETURN_EXPR,
    _FOR_WITHOUT_YIELD,
    _FOR_WITH_YIELD,
    _INFIX,
    _ASSIGNMENT,
};


enum SimpleExprType {
    _INSTANCE_CREATING,
    _BLOCK_STATS,
    _SIMPLE_EXPR_1,
};

enum SimpleExpr1Type {
    _INTEGER_LITERAL,
    _CHAR_LITERAL,
    _DOUBLE_LITERAL,
    _STRING_LITERAL,
    _BOOL_LITERAL,
    _NULL_LITERAL,
    _IDENTIFIER,
    _SUPER_FIELD_ACCESS,
    _THIS_FIELD_ACCESS,
    _EXPRESSION_FIELD_ACCESS,
    _ARGUMENT_CALL,
    _EMPTY_CALL,
    _METHOD_CALL,
};

enum PrefixExprType {
    _UNARY_PLUS,
    _UNARY_MINUS,
    _NOT,
    _BIT_NOT,
};

enum StableIdType {
    _STABLE_ID_IDENTIFIER,
    _STABLE_ID_SUPER_FIELD_ACCESS,
    _STABLE_ID_THIS_FIELD_ACCESS,
};

enum StatType {
    _TEMPLATE,
    _CLASS,
    _OBJECT,
    _TRAIT,
    _ENUM,
    _VAR_DECL,
    _VAL_DECL,
    _FUN_DEF,
};

enum ClassParamType {
    _VAL_CLASS_PARAM,
    _VAR_CLASS_PARAM,
    _UNMARKED_CLASS_PARAM,
};

enum EnumCaseType {
    _CASE_WITH_EXTENDS,
    _CASE_WITH_PARAMS,
    _CASE_WITH_IDS,
};

enum DefinitionType {
    _VAR_DEFS,
    _FUN_DEFINITION,
    _TEMPLATE_DEFINITION,
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