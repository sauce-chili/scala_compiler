#include "Types.h"

std::string modifierToString(ModifierType m) {
    switch (m) {
        case PRIVATE:   return "PRIVATE";
        case PROTECTED: return "PROTECTED";
        case OPEN:      return "OPEN";
        case FINAL:     return "FINAL";
        case OVERRIDE:  return "OVERRIDE";
    }
}

std::string simpleExpr1ToString(SimpleExpr1Type t) {
    switch (t) {
        case IDENTIFIER:              return "Identifier";
        case SUPER_FIELD_ACCESS:      return "Super field access";
        case THIS_FIELD_ACCESS:       return "This field access";
        case EXPRESSION_FIELD_ACCESS: return "Expression field access";
        case ARGUMENT_CALL:           return "Argument call";
        case EMPTY_CALL:              return "Empty call";
        case METHOD_CALL:             return "Method call";
    }
}

std::string simpleExprToString(SimpleExprType t) {
    switch (t) {
        case OBJECT_CREATING: return "Object creating";
        case BLOCK_STATS:     return "Block statements";
        case SIMPLE_EXPR_1:   return "Simple expr 1";
    }
}

std::string stableIdTypeToString(StableIdType t) {
    switch (t) {
        case STABLE_ID_IDENTIFIER:             return "Stable id identifier";
        case STABLE_ID_SUPER_FIELD_ACCESS:     return "Stable id Super field access";
        case STABLE_ID_THIS_FIELD_ACCESS:      return "Stable id This field access";
    }
}

std::string statTypeToString(StatType t) {
    switch (t) {
        case TEMPLATE: return "Template";
        case ENUM:     return "Enum";
        case VAR_DECL: return "Var declaration";
        case VAL_DECL: return "Val declaration";
    }
}

std::string prefixExprToString(PrefixExprType t) {
    switch (t) {
        case UNARY_PLUS:  return "Unary plus";
        case UNARY_MINUS: return "Unary minus";
        case NOT:         return "Not";
        case BIT_NOT:     return "Bit not";

        default: return "No unary operator";
    }
}