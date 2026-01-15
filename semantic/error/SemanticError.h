#ifndef SCALA_LEXER_SEMANTICERROR_H
#define SCALA_LEXER_SEMANTICERROR_H

#include <string>
#include <stdexcept>

#include "SemanticErrorCode.h"

class SemanticError: public std::exception {
public:
    int code;
    std::string message;
    int line;

    SemanticError(SemanticErrorCode code, int line, const std::string& message);

    const char* what() const noexcept override;
    std::string getErrorMessage();

    // ===== AST =====
    static SemanticError EmptyAstTree(int line);

    // ===== Переменные =====
    static SemanticError VarRedefinition(int line, const std::string& name);
    static SemanticError ValReassignment(int line, const std::string& name);
    static SemanticError VarInvalidTypeCast(int line, const std::string& from, const std::string& to);
    static SemanticError FinalFieldOverride(int line, const std::string& name);
    static SemanticError AbstractFieldNotOverride(int line, const std::string& name);
    static SemanticError VarNotInitialized(int line, const std::string& name);
    static SemanticError UndefinedVar(int line, const std::string& name);
    static SemanticError FieldRedefinition(int line, const std::string& name);

    // ===== Типы =====
    static SemanticError TypeOverflow(int line, const std::string& type);
    static SemanticError DivideByZero(int line);
    static SemanticError TypeMismatch(int line, const std::string& expected, const std::string& actual);
    static SemanticError NullPointerAccess(int line);
    static SemanticError InvalidCombinationOfModifiers(int line, const std::string& mods);
    static SemanticError UndeclaredType(int line, const std::string& type);

    // ===== Массивы =====
    static SemanticError InvalidArrayAssignment(int line, const std::string& expected, const std::string& actual);
    static SemanticError InvalidDimensionSize(int line);
    static SemanticError IndexOutOfArrayBounds(int line, int index);
    static SemanticError HeterogeneousArrayTypes(int line);
    static SemanticError ForLoopNotArray(int line, const std::string& type);
    static SemanticError ExprNotArray(int line);
    static SemanticError IfBranchesReturnDifferentTypes(int line, const std::string& t1, const std::string& t2);

    // ===== Наследование / классы =====
    static SemanticError InvalidInheritance(int line, const std::string& base);
    static SemanticError FinalClassInheritance(int line, const std::string& base);
    static SemanticError UndefinedClass(int line, const std::string& name);
    static SemanticError ClassRedefinition(int line, const std::string& name);
    static SemanticError ConstructorAlreadyExists(int line, const std::string& signature);
    static SemanticError ConstructorNotFound(int line, const std::string& signature);
    static SemanticError ClassNotFound(int line, const std::string& name);
    static SemanticError AbstractClassInstantiated(int line, const std::string& name);
    static SemanticError ClassCanNotBeOverride(int line, const std::string& name);
    static SemanticError MainClassNotDefined(int line);

    // ===== Методы =====
    static SemanticError FinalMethodOverride(int line, const std::string& name);
    static SemanticError MethodAlreadyExists(int line, const std::string& signature);
    static SemanticError MissingReturnValue(int line);
    static SemanticError ReturnFromVoid(int line);
    static SemanticError ReturnTypeMismatch(int line, const std::string& expected, const std::string& actual);
    static SemanticError UnexpectedReturn(int line);
    static SemanticError ConditionNotBoolean(int line, const std::string& actual);
    static SemanticError MethodNotFound(int line, const std::string& name);
    static SemanticError MethodCandidateNotFound(int line, const std::string& name, const std::string& args);
    static SemanticError InvalidOperator(int line, const std::string& op, const std::string& left, const std::string& right);
    static SemanticError NullSemanticTypeForLeftExpr(int line);
    static SemanticError NullSemanticType(int line);
    static SemanticError SameArgumentsNames(int line, const std::string& name);
    static SemanticError MainMethodNotDefined(int line);
    static SemanticError AbstractMethodNotImplemented(int line, const std::string& name);
};


#endif //SCALA_LEXER_SEMANTICERROR_H
