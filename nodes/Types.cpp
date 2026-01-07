#include "Types.h"

std::string modifierToString(ModifierType m) {
    switch (m) {
        case _PRIVATE:   return "PRIVATE";
        case _PROTECTED: return "PROTECTED";
        case _ABSTRACT:  return "ABSTRACT";
        case _SEALED:    return "SEALED";
        case _FINAL:     return "FINAL";
        case _OVERRIDE:  return "OVERRIDE";
    }
}

std::string simpleExpr1ToString(SimpleExpr1Type t) {
    switch (t) {
        case _IDENTIFIER:              return "Identifier";
        case _SUPER_FIELD_ACCESS:      return "Super field access";
        case _THIS_FIELD_ACCESS:       return "This field access";
        case _EXPRESSION_FIELD_ACCESS: return "Expression field access";
        case _ARGUMENT_CALL:           return "Argument call";
        case _EMPTY_CALL:              return "Empty call";
        case _METHOD_CALL:             return "Method call";
    }
}

std::string simpleExprToString(SimpleExprType t) {
    switch (t) {
        case _INSTANCE_CREATING: return "Object creating";
        case _BLOCK_STATS:     return "Block statements";
        case _SIMPLE_EXPR_1:   return "Simple expr 1";
    }
}

std::string stableIdTypeToString(StableIdType t) {
    switch (t) {
        case _STABLE_ID_IDENTIFIER:             return "Stable id identifier";
        case _STABLE_ID_SUPER_FIELD_ACCESS:     return "Stable id Super field access";
        case _STABLE_ID_THIS_FIELD_ACCESS:      return "Stable id This field access";
    }
}

std::string statTypeToString(StatType t) {
    switch (t) {
        case _TEMPLATE: return "Template";
        case _CLASS:    return "Class";
        case _OBJECT:   return "Object";
        case _TRAIT:    return "Trait";
        case _ENUM:     return "Enum";
        case _VAR_DECL: return "Var declaration";
        case _VAL_DECL: return "Val declaration";
        case _FUN_DEF:  return "Function definition";
    }
}

std::string prefixExprToString(PrefixExprType t) {
    switch (t) {
        case _UNARY_PLUS:  return "Unary plus";
        case _UNARY_MINUS: return "Unary minus";
        case _NOT:         return "Not";
        case _BIT_NOT:     return "Bit not";

        default: return "No unary operator";
    }
}

std::string classParamTypeToString(ClassParamType t) {
    switch (t) {
        case _VAL_CLASS_PARAM:      return "Val class param";
        case _VAR_CLASS_PARAM:      return "Var class param";
        case _UNMARKED_CLASS_PARAM: return "Unmarked class param";
    }
}

std::string classEnumCaseTypeToString(EnumCaseType t) {
    switch (t) {
        case _CASE_WITH_EXTENDS: return "Case with extends";
        case _CASE_WITH_PARAMS:  return "Case with params";
        case _CASE_WITH_IDS:     return "Case with ids list";
    }
}

std::string definitionTypeToString(DefinitionType t) {
    switch (t) {
        case _VAR_DEFS:            return "Variables definition";
        case _FUN_DEFINITION:      return "Function definition";
        case _TEMPLATE_DEFINITION: return "Template definition";
    }
}

std::string exprTypeToString(ExprType t) {
    switch (t) {
        case _IF_ELSE:           return "If Else";
        case _IF:                return "If";
        case _WHILE:             return "While";
        case _TRY:               return "Try";
        case _DO_WHILE:          return "Do While";
        case _THROW:             return "Throw";
        case _RETURN_EMPTY:      return "Return";
        case _RETURN_EXPR:       return "Return expr";
        case _FOR_WITHOUT_YIELD: return "For";
        case _FOR_WITH_YIELD:    return "For Yield";
        case _INFIX:             return "Infix";
        case _ASSIGNMENT:        return "Assignment";
    }
}