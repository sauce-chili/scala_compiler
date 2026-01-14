#include "SemanticError.h"

SemanticError::SemanticError(SemanticErrorCode code, int line, const std::string &message) {

}

std::string SemanticError::getErrorMessage() {
    return "Line " + std::to_string(line) + ": " + message;
}

const char *SemanticError::what() const noexcept {
    return exception::what();
}

// ------------------------------------------ AST ------------------------------------------
SemanticError SemanticError::EmptyAstTree(int line) {
    return SemanticError(EMPTY_AST_TREE, line, "AST tree is empty");
}

// ------------------------------------------ Переменные ------------------------------------------
SemanticError SemanticError::VarRedefinition(int line, const std::string& name) {
    return SemanticError(VAR_REDEFINITION, line, "Variable '" + name + "' is already defined");
}

SemanticError SemanticError::ValReassignment(int line, const std::string& name) {
    return SemanticError(VAL_REASSIGNMENT, line, "Cannot reassign immutable value '" + name + "'");
}

SemanticError SemanticError::VarInvalidTypeCast(int line, const std::string& from, const std::string& to) {
    return SemanticError(VAR_INVALID_TYPE_CAST, line, "Invalid cast from '" + from + "' to '" + to + "'");
}

SemanticError SemanticError::FinalFieldOverride(int line, const std::string& name) {
    return SemanticError(FINAL_FIELD_OVERRIDE, line, "Cannot override final field '" + name + "'");
}

SemanticError SemanticError::AbstractFieldNotOverride(int line, const std::string& name) {
    return SemanticError(ABSTRACT_FIELD_NOT_OVERRIDEN, line, "Abstract field '" + name + "' must be overridden");
}

SemanticError SemanticError::VarNotInitialized(int line, const std::string& name) {
    return SemanticError(VAR_NOT_INITIALIZED, line, "Variable '" + name + "' is not initialized");
}

SemanticError SemanticError::UndefinedVar(int line, const std::string& name) {
    return SemanticError(UNDEFINED_VAR, line, "Undefined variable '" + name + "'");
}

SemanticError SemanticError::FieldRedefinition(int line, const std::string& name) {
    return SemanticError(FIELD_REDEFINITION, line, "Field '" + name + "' is already defined");
}

// ------------------------------------------ Типы ------------------------------------------
SemanticError SemanticError::TypeOverflow(int line, const std::string& type) {
    return SemanticError(TYPE_OVERFLOW, line, "Value overflow for type '" + type + "'");
}

SemanticError SemanticError::DivideByZero(int line) {
    return SemanticError(DIVIDE_BY_ZERO, line, "Division by zero");
}

SemanticError SemanticError::TypeMismatch(int line, const std::string& expected, const std::string& actual) {
    return SemanticError(TYPE_MISMATCH, line, "Type mismatch: expected '" + expected + "', got '" + actual + "'");
}

SemanticError SemanticError::NullPointerAccess(int line) {
    return SemanticError(NULL_POINTER_ACCESS, line, "Null pointer access");
}

SemanticError SemanticError::InvalidCombinationOfModifiers(int line, const std::string& mods) {
    return SemanticError(INVALID_COMBINATION_OF_MODIFIERS, line, "Invalid combination of modifiers: " + mods);
}

SemanticError SemanticError::UndeclaredType(int line, const std::string& type) {
    return SemanticError(UNDECLARED_TYPE, line, "Type '" + type + "' is not declared");
}

// ------------------------------------------ Массивы ------------------------------------------
SemanticError SemanticError::InvalidArrayAssignment(int line, const std::string& expected, const std::string& actual) {
    return SemanticError(INVALID_ARRAY_ASSIGNMENT, line, "Invalid array assignment: expected '" + expected + "', got '" + actual + "'");
}

SemanticError SemanticError::InvalidDimensionSize(int line) {
    return SemanticError(INVALID_DIMENSION_SIZE, line, "Array dimension size must be positive");
}

SemanticError SemanticError::IndexOutOfArrayBounds(int line, int index) {
    return SemanticError(INDEX_OUT_OF_ARRAY_BOUNDS, line, "Index " + std::to_string(index) + " is out of array bounds");
}

SemanticError SemanticError::HeterogeneousArrayTypes(int line) {
    return SemanticError(HETEROGENEOUS_ARRAY_TYPES, line, "Array elements must have the same type");
}

SemanticError SemanticError::ForLoopNotArray(int line, const std::string& type) {
    return SemanticError(FOR_LOOP_NOT_ARRAY, line, "For-loop expects array, but got '" + type + "'");
}

SemanticError SemanticError::ExprNotArray(int line) {
    return SemanticError(EXPR_NOT_ARRAY, line, "Expression is not an array");
}

SemanticError SemanticError::IfBranchesReturnDifferentTypes(int line, const std::string& t1, const std::string& t2) {
    return SemanticError(IF_BRANCHES_RETURNS_DIFFERENT_TYPES, line, "If branches return different types: '" + t1 + "' and '" + t2 + "'");
}

// ------------------------------------------ Наследование / классы ------------------------------------------
SemanticError SemanticError::InvalidInheritance(int line, const std::string& base) {
    return SemanticError(INVALID_INHERITANCE, line, "Invalid inheritance from '" + base + "'");
}

SemanticError SemanticError::FinalClassInheritance(int line, const std::string& base) {
    return SemanticError(FINAL_CLASS_INHERITANCE, line, "Cannot inherit from final class '" + base + "'");
}

SemanticError SemanticError::UndefinedClass(int line, const std::string& name) {
    return SemanticError(UNDEFINED_CLASS, line, "Undefined class '" + name + "'");
}

SemanticError SemanticError::ClassRedefinition(int line, const std::string& name) {
    return SemanticError(CLASS_REDEFINITION, line, "Class '" + name + "' is already defined");
}

SemanticError SemanticError::ConstructorAlreadyExists(int line, const std::string& signature) {
    return SemanticError(CONSTRUCTOR_ALREADY_EXISTS, line, "Constructor '" + signature + "' already exists");
}

SemanticError SemanticError::ConstructorNotFound(int line, const std::string& signature) {
    return SemanticError(CONSTRUCTOR_NOT_FOUND, line, "Constructor '" + signature + "' not found");
}

SemanticError SemanticError::ClassNotFound(int line, const std::string& name) {
    return SemanticError(CLASS_NOT_FOUND, line, "Class '" + name + "' not found");
}

SemanticError SemanticError::AbstractClassInstantiated(int line, const std::string& name) {
    return SemanticError(ABSTRACT_CLASS_INSTANTIATED, line, "Cannot instantiate abstract class '" + name + "'");
}

SemanticError SemanticError::ClassCanNotBeOverride(int line, const std::string& name) {
    return SemanticError(CLASS_CAN_NOT_BE_OVERRIDE, line, "Class '" + name + "' cannot be overridden");
}

SemanticError SemanticError::MainClassNotDefined(int line) {
    return SemanticError(MAIN_CLASS_NOT_DEFINED, line, "Main class is not defined");
}

// ------------------------------------------ Методы ------------------------------------------
SemanticError SemanticError::FinalMethodOverride(int line, const std::string& name) {
    return SemanticError(FINAL_METHOD_OVERRIDE, line, "Cannot override final method '" + name + "'");
}

SemanticError SemanticError::MethodAlreadyExists(int line, const std::string& signature) {
    return SemanticError(METHOD_ALREADY_EXISTS, line, "Method '" + signature + "' already exists");
}

SemanticError SemanticError::MissingReturnValue(int line) {
    return SemanticError(MISSING_RETURN_VALUE, line, "Missing return value");
}

SemanticError SemanticError::ReturnFromVoid(int line) {
    return SemanticError(RETURN_FROM_VOID, line, "Cannot return a value from void method");
}

SemanticError SemanticError::ReturnTypeMismatch(int line, const std::string& expected, const std::string& actual) {
    return SemanticError(RETURN_TYPE_MISMATCH, line, "Return type mismatch: expected '" + expected + "', got '" + actual + "'");
}

SemanticError SemanticError::UnexpectedReturn(int line) {
    return SemanticError(UNEXPECTED_RETURN, line, "Unexpected return statement");
}

SemanticError SemanticError::ConditionNotBoolean(int line, const std::string& actual) {
    return SemanticError(CONDITION_NOT_BOOLEAN, line, "Condition must be boolean, but got '" + actual + "'");
}

SemanticError SemanticError::MethodNotFound(int line, const std::string& name) {
    return SemanticError(METHOD_NOT_FOUND, line, "Method '" + name + "' not found");
}

SemanticError SemanticError::MethodCandidateNotFound(int line, const std::string& name, const std::string& args) {
    return SemanticError(METHOD_CANDIDATE_NOT_FOUND, line, "No matching method '" + name + "' for argument types (" + args + ")");
}

SemanticError SemanticError::InvalidOperator(int line, const std::string& op, const std::string& left, const std::string& right) {
    return SemanticError(INVALID_OPERATOR, line, "Operator '" + op + "' cannot be applied to types '" + left + "' and '" + right + "'");
}

SemanticError SemanticError::NullSemanticTypeForLeftExpr(int line) {
    return SemanticError(NULL_SEMANTIC_TYPE_FOR_LEFT_EXPR, line, "Left expression has null semantic type");
}

SemanticError SemanticError::NullSemanticType(int line) {
    return SemanticError(NULL_SEMANTIC_TYPE, line, "Expression has null semantic type");
}

SemanticError SemanticError::SameArgumentsNames(int line, const std::string& name) {
    return SemanticError(SAME_ARGUMENTS_NAMES, line, "Duplicate argument name '" + name + "'");
}

SemanticError SemanticError::MainMethodNotDefined(int line) {
    return SemanticError(MAIN_METHOD_NOT_DEFINED, line, "Main method is not defined");
}

SemanticError SemanticError::AbstractMethodNotImplemented(int line, const std::string &name) {
    return SemanticError(ABSTRACT_METHOD_NOT_IMPLEMENTED, line, "Main method '" + name + "' is not defined");
}