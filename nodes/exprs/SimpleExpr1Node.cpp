#include "SimpleExpr1Node.h"
#include "ArgumentExprsNode.h"
#include "SimpleExprNode.h"
#include "ExprNode.h"
#include "ExprsNode.h"
#include "../id/IdNode.h"
#include "../type/SimpleTypeNode.h"
#include "semantic/SemanticContext.h"
#include "semantic/tables/tables.hpp"
#include "semantic/scopes/Scope.h"
#include "semantic/error/ErrorTable.h"
#include "semantic/error/SemanticError.h"

SimpleExpr1Node::SimpleExpr1Node() {
    intValue = 0;
    charValue = '\0';
    doubleValue = 0.0;
    stringValue = "";
    boolValue = false;

    simpleExpr = nullptr;
    expr = nullptr;
    argumentExprs = nullptr;
    simpleExpr1 = nullptr;
    identifier = nullptr;
    simpleType = nullptr;
}

SimpleExpr1Node* SimpleExpr1Node::createIntNode(int value) {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = _INTEGER_LITERAL;
    node->intValue = value;
    return node;
}

SimpleExpr1Node* SimpleExpr1Node::createCharNode(char value) {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = _CHAR_LITERAL;
    node->charValue = value;
    return node;
}

SimpleExpr1Node* SimpleExpr1Node::createDoubleNode(double value) {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = _DOUBLE_LITERAL;
    node->doubleValue = value;
    return node;
}

SimpleExpr1Node* SimpleExpr1Node::createStringNode(string value) {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = _STRING_LITERAL;
    node->stringValue = value;
    return node;
}

SimpleExpr1Node* SimpleExpr1Node::createBoolNode(bool value) {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = _BOOL_LITERAL;
    node->boolValue = value;
    return node;
}

SimpleExpr1Node* SimpleExpr1Node::createNullNode() {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = _NULL_LITERAL;
    return node;
}

SimpleExpr1Node *SimpleExpr1Node::createIdNode(IdNode* identifier) {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = _IDENTIFIER;
    node->identifier = identifier;
    return node;
}

SimpleExpr1Node* SimpleExpr1Node::createMethodCallNode(SimpleExpr1Node* left, ArgumentExprsNode* right) {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = _METHOD_CALL;
    node->simpleExpr1 = left;
    node->argumentExprs = right;
    return node;
}

SimpleExpr1Node* SimpleExpr1Node::createSuperFieldAccessNode(IdNode* identifier) {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = _SUPER_FIELD_ACCESS;
    node->identifier = identifier;
    return node;
}

SimpleExpr1Node* SimpleExpr1Node::createThisFieldAccessNode(IdNode* identifier) {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = _THIS_FIELD_ACCESS;
    node->identifier = identifier;
    return node;
}

SimpleExpr1Node *SimpleExpr1Node::createSimpleExprFieldAccessNode(IdNode* identifier, SimpleExprNode* expr) {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = _EXPRESSION_FIELD_ACCESS;
    node->identifier = identifier;
    node->simpleExpr = expr;
    return node;
}

SimpleExpr1Node* SimpleExpr1Node::createParenthesizedExprNode(ExprNode* expr) {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = _PARENTHESIZED_EXPR;
    node->expr = expr;
    return node;
}

SimpleExpr1Node* SimpleExpr1Node::createUnitLiteralNode() {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = _UNIT_LITERAL;
    return node;
}

SimpleExpr1Node *SimpleExpr1Node::createPlainThisNode() {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = _PLAIN_THIS;
    return node;
}

SimpleExpr1Node *SimpleExpr1Node::createArrayWithTypeBuilderNode(SimpleTypeNode *simpleType,
                                                                 ArgumentExprsNode *argumentExprs) {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = _ARRAY_WITH_TYPE_BUILDER;
    node->simpleType = simpleType;
    node->argumentExprs = argumentExprs;
    return node;
}

SimpleExpr1Node *SimpleExpr1Node::createArrayBuilderNode(ArgumentExprsNode *argumentExprs) {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = _ARRAY_BUILDER;
    node->argumentExprs = argumentExprs;
    return node;
}

bool SimpleExpr1Node::isLiteral() const {
    return type == _INTEGER_LITERAL || type == _DOUBLE_LITERAL ||
           type == _STRING_LITERAL || type == _CHAR_LITERAL ||
           type == _BOOL_LITERAL;
}

SimpleExpr1Node *SimpleExpr1Node::copy() {
    SimpleExpr1Node* node = new SimpleExpr1Node();
    node->type = type;
    node->intValue = intValue;
    node->charValue = charValue;
    node->doubleValue = doubleValue;
    node->stringValue = stringValue;
    node->boolValue = boolValue;

    if (simpleExpr) {
        node->simpleExpr = simpleExpr->copy();
    }
    if (expr) {
        node->expr = expr->copy();
    }
    if (argumentExprs) {
        node->argumentExprs = argumentExprs->copy();
    }
    if (simpleExpr1) {
        node->simpleExpr1 = simpleExpr1->copy();
    }
    if (identifier) {
        node->identifier = identifier->copy();
    }
    if (simpleType) {
        node->simpleType = simpleType->copy();
    }

    return node;
}

string SimpleExpr1Node::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, simpleExpr, "simple expr");
    addDotChild(dot, expr, "expr");
    addDotChild(dot, argumentExprs, "argument exprs");
    addDotChild(dot, simpleExpr1, "simple expr 1");
    addDotChild(dot, identifier, "identifier");
    addDotChild(dot, simpleType, "simpleType");

    return dot;
}

string SimpleExpr1Node::getDotLabel() const {
    switch (type) {
        case _INTEGER_LITERAL:    return to_string(intValue);
        case _DOUBLE_LITERAL:     return to_string(doubleValue);
        case _BOOL_LITERAL:       return to_string(boolValue);
        case _CHAR_LITERAL :      return to_string(charValue);
        case _STRING_LITERAL:     return stringValue;
        case _NULL_LITERAL:       return "null";

        default: break;
    }

    return simpleExpr1ToString(type);
}

list<Node *> SimpleExpr1Node::getChildren() const {
    list<Node *> children = {};
    addChildIfNotNull(children, simpleExpr);
    addChildIfNotNull(children, expr); // '(' expr ')'
    addChildIfNotNull(children, argumentExprs);
    addChildIfNotNull(children, simpleExpr1);
    addChildIfNotNull(children, identifier);
    addChildIfNotNull(children, simpleType);
    return children;
}

DataType SimpleExpr1Node::inferType(
    ClassMetaInfo* currentClass,
    MethodMetaInfo* currentMethod,
    Scope* currentScope
) const {
    switch (type) {
        case _INTEGER_LITERAL:
            return DataType::makeInt();

        case _DOUBLE_LITERAL:
            return DataType::makeDouble();

        case _CHAR_LITERAL:
            return DataType::makeChar();

        case _STRING_LITERAL:
            return DataType::makeString();

        case _BOOL_LITERAL:
            return DataType::makeBoolean();

        case _NULL_LITERAL:
            return DataType::makeNull();

        case _IDENTIFIER:
            if (identifier) {
                std::string name = identifier->name;

                // Search for local variable
                if (currentMethod) {
                    auto localOpt = currentMethod->resolveLocal(name, currentScope);
                    if (localOpt.has_value()) {
                        return localOpt.value()->dataType;
                    }
                }

                // Search for class field
                if (currentClass) {
                    auto fieldOpt = currentClass->resolveField(name, currentClass);
                    if (fieldOpt.has_value()) {
                        return fieldOpt.value()->dataType;
                    }
                }

                // Not a variable or field - error (methods are handled in _METHOD_CALL)
                throw SemanticError::UndefinedVar(identifier->id, name);
            }
            throw SemanticError::InternalError(id, "SimpleExpr1Node _IDENTIFIER without identifier");

        case _THIS_FIELD_ACCESS:
            if (identifier && currentClass) {
                std::string fieldName = identifier->name;
                auto fieldOpt = currentClass->resolveField(fieldName, currentClass);
                if (fieldOpt.has_value()) {
                    return fieldOpt.value()->dataType;
                }
                throw SemanticError::UndefinedVar(identifier->id, fieldName);
            }
            throw SemanticError::InternalError(id, "SimpleExpr1Node _THIS_FIELD_ACCESS invalid state");

        case _SUPER_FIELD_ACCESS:
            if (identifier && currentClass && currentClass->parent) {
                std::string fieldName = identifier->name;
                auto fieldOpt = currentClass->parent->resolveField(fieldName, currentClass);
                if (fieldOpt.has_value()) {
                    return fieldOpt.value()->dataType;
                }
                throw SemanticError::UndefinedVar(identifier->id, fieldName);
            }
            throw SemanticError::InternalError(id, "SimpleExpr1Node _SUPER_FIELD_ACCESS invalid state");

        case _EXPRESSION_FIELD_ACCESS:
            if (simpleExpr && identifier) {
                DataType receiverType = simpleExpr->inferType(currentClass, currentMethod, currentScope);
                std::string fieldName = identifier->name;

                std::string className = receiverType.getClassName();
                auto classIt = ctx().classes.find(className);
                if (classIt != ctx().classes.end()) {
                    auto fieldOpt = classIt->second->resolveField(fieldName, currentClass);
                    if (fieldOpt.has_value()) {
                        return fieldOpt.value()->dataType;
                    }
                    throw SemanticError::UndefinedVar(identifier->id, fieldName);
                }
                throw SemanticError::UndefinedClass(id, className);
            }
            throw SemanticError::InternalError(id, "SimpleExpr1Node _EXPRESSION_FIELD_ACCESS invalid state");

        case _METHOD_CALL: {
            if (!simpleExpr1) {
                throw SemanticError::InternalError(id, "SimpleExpr1Node _METHOD_CALL without simpleExpr1");
            }
            // argumentExprs can be null for synthetic calls (super constructor)
            if (!argumentExprs) {
                return DataType::makeUnit();
            }

            auto argTypes = argumentExprs->getArgsTypes(currentClass, currentMethod, currentScope);
            SimpleExpr1Node* callee = simpleExpr1;

                if (callee->type == _EXPRESSION_FIELD_ACCESS && callee->identifier) {
                    std::string methodName = callee->identifier->name;
                    DataType objType = callee->simpleExpr->inferType(currentClass, currentMethod, currentScope);

                    std::string className = objType.getClassName();
                    auto classIt = ctx().classes.find(className);
                    if (classIt != ctx().classes.end()) {
                        auto methodOpt = classIt->second->resolveMethod(methodName, argTypes, currentClass);
                        if (methodOpt.has_value()) {
                            DataType returnType = methodOpt.value()->returnType;
                            for (auto* t : argTypes) delete t;
                            return returnType;
                        }

                        std::string argsStr = "(";
                        for (size_t i = 0; i < argTypes.size(); ++i) {
                            if (i > 0) argsStr += ", ";
                            argsStr += argTypes[i]->toString();
                        }
                        argsStr += ")";
                        for (auto* t : argTypes) delete t;
                        throw SemanticError::MethodCandidateNotFound(id, methodName, argsStr);
                    }

                    for (auto* t : argTypes) delete t;
                    throw SemanticError::UndefinedClass(id, className);

                } else if (callee->type == _IDENTIFIER && callee->identifier) {
                    std::string methodName = callee->identifier->name;
                    if (currentClass) {
                        auto methodOpt = currentClass->resolveMethod(methodName, argTypes, currentClass);
                        if (methodOpt.has_value()) {
                            DataType returnType = methodOpt.value()->returnType;
                            for (auto* t : argTypes) delete t;
                            return returnType;
                        }

                        std::string argsStr = "(";
                        for (size_t i = 0; i < argTypes.size(); ++i) {
                            if (i > 0) argsStr += ", ";
                            argsStr += argTypes[i]->toString();
                        }
                        argsStr += ")";
                        for (auto* t : argTypes) delete t;
                        throw SemanticError::MethodCandidateNotFound(id, methodName, argsStr);
                    }

                    for (auto* t : argTypes) delete t;
                    throw SemanticError::InternalError(id, "Method call without currentClass");

                } else if (callee->type == _THIS_FIELD_ACCESS && callee->identifier) {
                    std::string methodName = callee->identifier->name;
                    if (currentClass) {
                        auto methodOpt = currentClass->resolveMethod(methodName, argTypes, currentClass);
                        if (methodOpt.has_value()) {
                            DataType returnType = methodOpt.value()->returnType;
                            for (auto* t : argTypes) delete t;
                            return returnType;
                        }

                        std::string argsStr = "(";
                        for (size_t i = 0; i < argTypes.size(); ++i) {
                            if (i > 0) argsStr += ", ";
                            argsStr += argTypes[i]->toString();
                        }
                        argsStr += ")";
                        for (auto* t : argTypes) delete t;
                        throw SemanticError::MethodCandidateNotFound(id, methodName, argsStr);
                    }

                    for (auto* t : argTypes) delete t;
                    throw SemanticError::InternalError(id, "This method call without currentClass");

                } else if (callee->type == _SUPER_FIELD_ACCESS && callee->identifier) {
                    std::string methodName = callee->identifier->name;
                    if (currentClass && currentClass->parent) {
                        auto methodOpt = currentClass->parent->resolveMethod(methodName, argTypes, currentClass);
                        if (methodOpt.has_value()) {
                            DataType returnType = methodOpt.value()->returnType;
                            for (auto* t : argTypes) delete t;
                            return returnType;
                        }

                        std::string argsStr = "(";
                        for (size_t i = 0; i < argTypes.size(); ++i) {
                            if (i > 0) argsStr += ", ";
                            argsStr += argTypes[i]->toString();
                        }
                        argsStr += ")";
                        for (auto* t : argTypes) delete t;
                        throw SemanticError::MethodCandidateNotFound(id, methodName, argsStr);
                    }

                    for (auto* t : argTypes) delete t;
                    throw SemanticError::InternalError(id, "Super method call without parent");

                } else if (callee->type == _METHOD_CALL) {
                    // Chain of method calls: a.b().c() - callee is the result of a.b()
                    // The result type of the inner call determines the class to search for the method
                    DataType calleeType = callee->inferType(currentClass, currentMethod, currentScope);

                    // This is calling apply() on the result - not supported yet
                    // For now, just return the callee type (this handles cases like factory methods)
                    for (auto* t : argTypes) delete t;
                    throw SemanticError::InternalError(id, "Chained method call with arguments not supported");
                }

                for (auto* t : argTypes) delete t;
                throw SemanticError::InternalError(id, "Unknown method call type: " + std::to_string(callee->type));
        }

        case _PLAIN_THIS:
            if (currentClass) {
                return DataType::makeClass(currentClass->name);
            }
            throw SemanticError::InternalError(id, "SimpleExpr1Node _PLAIN_THIS without currentClass");

        case _ARRAY_BUILDER:
        case _ARRAY_WITH_TYPE_BUILDER:
            if (simpleType) {
                DataType elemType = DataType::createFromNode(simpleType);
                return DataType::makeArray(elemType);
            } else if (argumentExprs) {
                auto argTypes = argumentExprs->getArgsTypes(currentClass, currentMethod, currentScope);
                if (!argTypes.empty()) {
                    DataType elemType = *argTypes[0];
                    for (auto* t : argTypes) delete t;
                    return DataType::makeArray(elemType);
                }
                for (auto* t : argTypes) delete t;
            }
            return DataType::makeArray(DataType(DataType::Kind::Any));

        case _PARENTHESIZED_EXPR:
            if (expr) {
                return expr->inferType(currentClass, currentMethod, currentScope);
            }
            throw SemanticError::InternalError(id, "SimpleExpr1Node _PARENTHESIZED_EXPR without expr");

        case _UNIT_LITERAL:
            return DataType::makeUnit();

        default:
            throw SemanticError::InternalError(id, "Unknown SimpleExpr1Node type");
    }
}
