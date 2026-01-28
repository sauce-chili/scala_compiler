//
// TypeCheckVisitor - проверяет типы выражений и присваиваний
//

#include "TypeCheckVisitor.h"

#include "nodes/nodes.h"
#include "nodes/func/FunDefNode.h"
#include "nodes/func/FunSigNode.h"
#include "nodes/func/FuncParamsNode.h"
#include "nodes/class/ClassDefNode.h"
#include "nodes/stats/BlockStatsNode.h"
#include "nodes/stats/BlockStatNode.h"
#include "nodes/var/VarDefsNode.h"
#include "nodes/exprs/ExprNode.h"
#include "nodes/exprs/InfixExprNode.h"
#include "nodes/exprs/PrefixExprNode.h"
#include "nodes/exprs/SimpleExprNode.h"
#include "nodes/exprs/SimpleExpr1Node.h"
#include "nodes/exprs/AssignmentNode.h"
#include "nodes/exprs/ArgumentExprsNode.h"
#include "nodes/exprs/ExprsNode.h"
#include "nodes/generator/EnumeratorsNode.h"
#include "nodes/generator/GeneratorNode.h"
#include "semantic/error/ErrorTable.h"
#include "semantic/error/SemanticError.h"

void TypeCheckVisitor::visitTree(TopStatSeqNode* root) {
    if (root) visit(root);
}

void TypeCheckVisitor::visit(Node* node) {
    if (!node) return;

    if (auto* classDef = dynamic_cast<ClassDefNode*>(node)) {
        visitClassDef(classDef);
        return;
    }

    if (currentClass) {
        if (auto* funDef = dynamic_cast<FunDefNode*>(node)) {
            visitFunDef(funDef);
            return;
        }
    }

    if (currentMethod) {
        if (auto* blockStats = dynamic_cast<BlockStatsNode*>(node)) {
            visitBlockStats(blockStats);
            return;
        }

        if (auto* expr = dynamic_cast<ExprNode*>(node); expr && expr->enumerators) {
            visitForExpression(expr);
            return;
        }
    }

    visitChildren(node);
}

void TypeCheckVisitor::visitChildren(Node* node) {
    for (Node* child : node->getChildren()) {
        visit(child);
    }
}

void TypeCheckVisitor::visitClassDef(ClassDefNode* node) {
    if (!node->fullId) return;

    std::string className = node->fullId->name;
    auto it = ctx().classes.find(className);
    if (it == ctx().classes.end()) return;

    ClassMetaInfo* prevClass = currentClass;
    currentClass = it->second;

    if (node->classTemplateOpt) {
        visit(node->classTemplateOpt);
    }

    currentClass = prevClass;
}

void TypeCheckVisitor::visitFunDef(FunDefNode* node) {
    if (!currentClass) return;

    std::string methodName;
    std::vector<DataType*> argTypes;

    if (node->funSig && node->funSig->fullId) {
        methodName = node->funSig->fullId->name;

        if (node->funSig->params && node->funSig->params->funcParams) {
            for (auto* paramNode : *node->funSig->params->funcParams) {
                DataType* argType = new DataType(DataType::createFromNode(paramNode->simpleType));
                argTypes.push_back(argType);
            }
        }
    } else if (node->isConstructor() && node->funcParams) {
        methodName = currentClass->name;
        if (node->funcParams->funcParams) {
            for (auto* paramNode : *node->funcParams->funcParams) {
                DataType* argType = new DataType(DataType::createFromNode(paramNode->simpleType));
                argTypes.push_back(argType);
            }
        }
    } else {
        return;
    }

    auto methodOpt = currentClass->resolveMethod(methodName, argTypes, currentClass);

    for (const auto* t : argTypes) delete t;

    if (!methodOpt.has_value()) return;

    MethodMetaInfo* prevMethod = currentMethod;
    Scope* prevScope = currentScope;

    currentMethod = methodOpt.value();
    currentScope = nullptr;

    if (node->expr) visit(node->expr);
    if (node->constrExpr) visit(node->constrExpr);

    currentMethod = prevMethod;
    currentScope = prevScope;
}

void TypeCheckVisitor::visitBlockStats(BlockStatsNode* node) {
    if (!currentMethod) return;

    Scope* prevScope = currentScope;
    currentScope = node->blockScope;

    if (node->blockStats) {
        for (auto* blockStat : *node->blockStats) {
            visitBlockStat(blockStat);
        }
    }

    currentScope = prevScope;
}

void TypeCheckVisitor::visitBlockStat(BlockStatNode* node) {
    if (!node) return;

    if (node->varDefs) {
        visitVarDefs(node->varDefs);
    }

    if (node->expr) {
        visitExpr(node->expr);
    }
}

void TypeCheckVisitor::visitVarDefs(VarDefsNode* node) {
    if (!node || !currentMethod) return;

    if (node->expr && node->simpleType) {
        DataType declaredType = DataType::createFromNode(node->simpleType);
        auto exprTypeOpt = inferExprType(node->expr);

        if (exprTypeOpt.has_value()) {
            int line = node->fullId ? node->fullId->id : 0;
            std::string varName = node->fullId ? node->fullId->name : "<unknown>";

            checkAssignmentCompatibility(declaredType, exprTypeOpt.value(), line,
                "Variable '" + varName + "' initialization");
        }
    }

    if (node->expr) {
        visitExpr(node->expr);
    }
}

void TypeCheckVisitor::visitExpr(ExprNode* node) {
    if (!node) return;

    switch (node->type) {
        case _ASSIGNMENT:
            if (node->assignment) {
                visitAssignment(node->assignment);
            }
            break;

        case _IF_ELSE:
        case _IF:
            if (node->exprs && !node->exprs->empty()) {
                auto it = node->exprs->begin();
                auto condTypeOpt = inferExprType(*it);
                if (condTypeOpt.has_value() && condTypeOpt->kind != DataType::Kind::Bool) {
                    ErrorTable::addErrorToList(new SemanticError(
                        SemanticError::ConditionNotBoolean(node->id, condTypeOpt->toString())
                    ));
                }
            }
            break;

        case _WHILE:
        case _DO_WHILE:
            if (node->exprs && !node->exprs->empty()) {
                auto it = node->exprs->begin();
                auto condTypeOpt = inferExprType(*it);
                if (condTypeOpt.has_value() && condTypeOpt->kind != DataType::Kind::Bool) {
                    ErrorTable::addErrorToList(new SemanticError(
                        SemanticError::ConditionNotBoolean(node->id, condTypeOpt->toString())
                    ));
                }
            }
            break;

        default:
            break;
    }

    visit(node);
}

void TypeCheckVisitor::visitAssignment(AssignmentNode* node) {
    if (!node || !currentMethod) return;

    int line = node->fullId ? node->fullId->id : node->id;

    // 1. ID_ASSIGNMENT: x = value
    if (node->fullId && !node->simpleExpr && !node->simpleExpr1) {
        std::string varName = node->fullId->name;
        auto exprTypeOpt = inferExprType(node->expr);

        auto localOpt = currentMethod->resolveLocal(varName, currentScope);
        if (localOpt.has_value()) {
            MethodVarMetaInfo* varInfo = localOpt.value();
            checkNotVal(varInfo->isVal, varName, line);
            if (exprTypeOpt.has_value()) {
                checkAssignmentCompatibility(varInfo->dataType, exprTypeOpt.value(), line,
                    "Assignment to variable '" + varName + "'");
            }
        } else {
            auto fieldOpt = currentClass->resolveField(varName, currentClass);
            if (fieldOpt.has_value()) {
                FieldMetaInfo* fieldInfo = fieldOpt.value();
                checkNotVal(fieldInfo->isVal, varName, line);
                if (exprTypeOpt.has_value()) {
                    checkAssignmentCompatibility(fieldInfo->dataType, exprTypeOpt.value(), line,
                        "Assignment to field '" + varName + "'");
                }
            } else {
                ErrorTable::addErrorToList(new SemanticError(
                    SemanticError::UndefinedVar(line, varName)
                ));
            }
        }
    }
    // 2. FIELD_ASSIGNMENT: obj.field = value
    else if (node->simpleExpr && node->fullId) {
        std::string fieldName = node->fullId->name;
        auto receiverTypeOpt = inferSimpleExprType(node->simpleExpr);
        auto exprTypeOpt = inferExprType(node->expr);

        if (receiverTypeOpt.has_value() && receiverTypeOpt->kind == DataType::Kind::Class) {
            auto classIt = ctx().classes.find(receiverTypeOpt->className);
            if (classIt != ctx().classes.end()) {
                ClassMetaInfo* receiverClass = classIt->second;
                auto fieldOpt = receiverClass->resolveField(fieldName, currentClass);

                if (fieldOpt.has_value()) {
                    FieldMetaInfo* fieldInfo = fieldOpt.value();
                    checkNotVal(fieldInfo->isVal, fieldName, line);
                    if (exprTypeOpt.has_value()) {
                        checkAssignmentCompatibility(fieldInfo->dataType, exprTypeOpt.value(), line,
                            "Assignment to field '" + fieldName + "'");
                    }
                } else {
                    ErrorTable::addErrorToList(new SemanticError(
                        SemanticError::UndefinedVar(line, fieldName)
                    ));
                }
            }
        }
    }
    // 3. ARRAY_ASSIGNMENT: arr(index) = value
    else if (node->simpleExpr1 && node->argumentExprs) {
        auto arrTypeOpt = inferSimpleExpr1Type(node->simpleExpr1);

        if (arrTypeOpt.has_value()) {
            if (arrTypeOpt->kind != DataType::Kind::Array) {
                ErrorTable::addErrorToList(new SemanticError(
                    SemanticError::ExprNotArray(line)
                ));
            } else {
                auto argTypes = collectArgumentTypes(node->argumentExprs);
                if (!argTypes.empty() && argTypes[0]->kind != DataType::Kind::Int) {
                    ErrorTable::addErrorToList(new SemanticError(
                        SemanticError::TypeMismatch(line, "Int", argTypes[0]->toString())
                    ));
                }

                auto exprTypeOpt = inferExprType(node->expr);
                if (arrTypeOpt->elementType && exprTypeOpt.has_value()) {
                    checkAssignmentCompatibility(*arrTypeOpt->elementType, exprTypeOpt.value(), line,
                        "Array element assignment");
                }

                for (auto* t : argTypes) delete t;
            }
        }
    }
}

void TypeCheckVisitor::visitForExpression(ExprNode* node) {
    if (!node || !node->enumerators || !currentMethod) return;

    Scope* prevScope = currentScope;
    currentScope = node->enumerators->scope;

    if (node->enumerators->generator && node->enumerators->generator->expr) {
        auto iterableTypeOpt = inferExprType(node->enumerators->generator->expr);
        if (iterableTypeOpt.has_value() && iterableTypeOpt->kind != DataType::Kind::Array) {
            ErrorTable::addErrorToList(new SemanticError(
                SemanticError::ForLoopNotArray(node->id, iterableTypeOpt->toString())
            ));
        }
    }

    if (node->exprs) {
        for (auto* expr : *node->exprs) {
            visit(expr);
        }
    }

    currentScope = prevScope;
}

// ================== Вывод типов ==================

std::optional<DataType> TypeCheckVisitor::inferExprType(ExprNode* expr) {
    if (!expr) return std::nullopt;

    switch (expr->type) {
        case _INFIX:
            if (expr->infixExpr) {
                return inferInfixExprType(expr->infixExpr);
            }
            break;

        case _ASSIGNMENT:
            return DataType::makeUnit();

        case _IF_ELSE:
            // Тип if-else - тип первой ветки (упрощение)
            if (expr->exprs && expr->exprs->size() >= 2) {
                auto it = expr->exprs->begin();
                ++it; // пропускаем условие
                return inferExprType(*it);
            }
            return DataType::makeUnit();

        case _IF:
        case _WHILE:
        case _DO_WHILE:
        case _FOR_WITHOUT_YIELD:
            return DataType::makeUnit();

        case _RETURN_EXPR:
            if (expr->exprs && !expr->exprs->empty()) {
                return inferExprType(expr->exprs->front());
            }
            return DataType::makeUnit();

        case _RETURN_EMPTY:
            return DataType::makeUnit();

        default:
            break;
    }

    return std::nullopt;
}

std::optional<DataType> TypeCheckVisitor::inferInfixExprType(InfixExprNode* infixExpr) {
    if (!infixExpr) return std::nullopt;

    // Если это просто prefix expression (после AST трансформации инфиксные операторы
    // преобразуются в вызовы методов, так что сюда попадает только PrefixExpr)
    if (infixExpr->prefixExpr && !infixExpr->left && !infixExpr->right) {
        return inferPrefixExprType(infixExpr->prefixExpr);
    }

    // Бинарная операция (если осталась после трансформации) - это вызов метода
    if (infixExpr->left && infixExpr->right && infixExpr->fullId) {
        auto leftTypeOpt = inferInfixExprType(infixExpr->left);
        auto rightTypeOpt = inferInfixExprType(infixExpr->right);

        if (!leftTypeOpt.has_value()) return std::nullopt;

        std::string methodName = infixExpr->fullId->name;

        // Ищем метод в классе левого операнда
        if (leftTypeOpt->kind == DataType::Kind::Class) {
            auto classIt = ctx().classes.find(leftTypeOpt->className);
            if (classIt != ctx().classes.end()) {
                std::vector<DataType*> argTypes;
                if (rightTypeOpt.has_value()) {
                    argTypes.push_back(new DataType(rightTypeOpt.value()));
                }

                auto methodOpt = classIt->second->resolveMethod(methodName, argTypes, currentClass);

                for (auto* t : argTypes) delete t;

                if (methodOpt.has_value()) {
                    return methodOpt.value()->returnType;
                }
            }
        }

        // Для примитивов ищем в RTL классах
        ClassMetaInfo* primitiveClass = nullptr;
        switch (leftTypeOpt->kind) {
            case DataType::Kind::Int: primitiveClass = RtlClassMetaInfo::Integer; break;
            case DataType::Kind::Double: primitiveClass = RtlClassMetaInfo::Double; break;
            case DataType::Kind::String: primitiveClass = RtlClassMetaInfo::String; break;
            case DataType::Kind::Bool: primitiveClass = RtlClassMetaInfo::Boolean; break;
            case DataType::Kind::Char: primitiveClass = RtlClassMetaInfo::Char; break;
            default: break;
        }

        if (primitiveClass) {
            std::vector<DataType*> argTypes;
            if (rightTypeOpt.has_value()) {
                argTypes.push_back(new DataType(rightTypeOpt.value()));
            }

            auto methodOpt = primitiveClass->resolveMethod(methodName, argTypes, currentClass);

            for (auto* t : argTypes) delete t;

            if (methodOpt.has_value()) {
                return methodOpt.value()->returnType;
            }
        }
    }

    return std::nullopt;
}

std::optional<DataType> TypeCheckVisitor::inferPrefixExprType(PrefixExprNode* prefixExpr) {
    if (!prefixExpr || !prefixExpr->simpleExpr) {
        return std::nullopt;
    }

    auto innerTypeOpt = inferSimpleExprType(prefixExpr->simpleExpr);
    if (!innerTypeOpt.has_value()) return std::nullopt;

    // Без унарного оператора - просто возвращаем тип
    if (prefixExpr->type == _NO_UNARY_OPERATOR) {
        return innerTypeOpt;
    }

    // Унарные операторы - ищем как методы
    std::string methodName;
    switch (prefixExpr->type) {
        case _UNARY_PLUS: methodName = "unary_+"; break;
        case _UNARY_MINUS: methodName = "unary_-"; break;
        case _NOT: methodName = "unary_!"; break;
        case _BIT_NOT: methodName = "unary_~"; break;
        default: return innerTypeOpt;
    }

    return resolveUnaryOperator(methodName, innerTypeOpt.value(), prefixExpr->simpleExpr->id);
}

std::optional<DataType> TypeCheckVisitor::resolveUnaryOperator(const std::string& op,
                                                                const DataType& operandType,
                                                                int line) {
    ClassMetaInfo* targetClass = nullptr;

    if (operandType.kind == DataType::Kind::Class) {
        auto classIt = ctx().classes.find(operandType.className);
        if (classIt != ctx().classes.end()) {
            targetClass = classIt->second;
        }
    } else {
        // Примитивные типы - ищем в RTL классах
        switch (operandType.kind) {
            case DataType::Kind::Int: targetClass = RtlClassMetaInfo::Integer; break;
            case DataType::Kind::Double: targetClass = RtlClassMetaInfo::Double; break;
            case DataType::Kind::Bool: targetClass = RtlClassMetaInfo::Boolean; break;
            case DataType::Kind::Char: targetClass = RtlClassMetaInfo::Char; break;
            default: break;
        }
    }

    if (targetClass) {
        std::vector<DataType*> emptyArgs;
        auto methodOpt = targetClass->resolveMethod(op, emptyArgs, currentClass);
        if (methodOpt.has_value()) {
            return methodOpt.value()->returnType;
        }
    }

    // Если метод не найден, возвращаем тип операнда (для совместимости)
    return operandType;
}

std::optional<DataType> TypeCheckVisitor::inferSimpleExprType(SimpleExprNode* simpleExpr) {
    if (!simpleExpr) return std::nullopt;

    switch (simpleExpr->type) {
        case _SIMPLE_EXPR_1:
            if (simpleExpr->simpleExpr1) {
                return inferSimpleExpr1Type(simpleExpr->simpleExpr1);
            }
            break;

        case _INSTANCE_CREATING:
            if (simpleExpr->fullId) {
                std::string className = simpleExpr->fullId->name;
                auto classIt = ctx().classes.find(className);
                if (classIt != ctx().classes.end()) {
                    ClassMetaInfo* classInfo = classIt->second;

                    if (classInfo->modifiers.hasModifier(_ABSTRACT)) {
                        ErrorTable::addErrorToList(new SemanticError(
                            SemanticError::AbstractClassInstantiated(simpleExpr->id, className)
                        ));
                    }

                    auto argTypes = collectArgumentTypes(simpleExpr->arguments);
                    auto ctorOpt = classInfo->resolveMethod(className, argTypes, currentClass);

                    if (!ctorOpt.has_value() && !argTypes.empty()) {
                        std::string argsStr = "(";
                        for (size_t i = 0; i < argTypes.size(); ++i) {
                            if (i > 0) argsStr += ", ";
                            argsStr += argTypes[i]->toString();
                        }
                        argsStr += ")";
                        ErrorTable::addErrorToList(new SemanticError(
                            SemanticError::ConstructorNotFound(simpleExpr->id, className + argsStr)
                        ));
                    }

                    for (auto* t : argTypes) delete t;
                    return DataType::makeClass(className);
                }
            }
            break;

        case _ARRAY_CREATING:
            if (simpleExpr->simpleType) {
                DataType elemType = DataType::createFromNode(simpleExpr->simpleType);
                return DataType::makeArray(elemType);
            }
            break;

        case _BLOCK_STATS:
            if (simpleExpr->blockStats) {
                return inferBlockStatsType(simpleExpr->blockStats);
            }
            break;
    }

    return std::nullopt;
}

std::optional<DataType> TypeCheckVisitor::inferBlockStatsType(BlockStatsNode* blockStats) {
    if (!blockStats || !blockStats->blockStats || blockStats->blockStats->empty()) {
        return DataType::makeUnit();
    }

    // Тип блока - тип последнего statement'а
    BlockStatNode* lastStat = blockStats->blockStats->back();

    if (lastStat->expr) {
        return inferExprType(lastStat->expr);
    }

    // Если последний statement - определение переменной, тип Unit
    return DataType::makeUnit();
}

std::optional<DataType> TypeCheckVisitor::inferSimpleExpr1Type(SimpleExpr1Node* simpleExpr1) {
    if (!simpleExpr1) return std::nullopt;

    switch (simpleExpr1->type) {
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
            if (simpleExpr1->identifier) {
                std::string name = simpleExpr1->identifier->name;

                // Ищем локальную переменную
                if (currentMethod) {
                    auto localOpt = currentMethod->resolveLocal(name, currentScope);
                    if (localOpt.has_value()) {
                        return localOpt.value()->dataType;
                    }
                }

                // Ищем поле класса
                if (currentClass) {
                    auto fieldOpt = currentClass->resolveField(name, currentClass);
                    if (fieldOpt.has_value()) {
                        return fieldOpt.value()->dataType;
                    }
                }

                // Не переменная и не поле - ошибка
                // (методы обрабатываются в _METHOD_CALL)
                ErrorTable::addErrorToList(new SemanticError(
                    SemanticError::UndefinedVar(simpleExpr1->identifier->id, name)
                ));
            }
            break;

        case _THIS_FIELD_ACCESS:
            if (simpleExpr1->identifier && currentClass) {
                std::string fieldName = simpleExpr1->identifier->name;
                auto fieldOpt = currentClass->resolveField(fieldName, currentClass);
                if (fieldOpt.has_value()) {
                    return fieldOpt.value()->dataType;
                }
                // Поле не найдено - ошибка
                ErrorTable::addErrorToList(new SemanticError(
                    SemanticError::UndefinedVar(simpleExpr1->identifier->id, fieldName)
                ));
            }
            break;

        case _SUPER_FIELD_ACCESS:
            if (simpleExpr1->identifier && currentClass && currentClass->parent) {
                std::string fieldName = simpleExpr1->identifier->name;
                auto fieldOpt = currentClass->parent->resolveField(fieldName, currentClass);
                if (fieldOpt.has_value()) {
                    return fieldOpt.value()->dataType;
                }
                // Поле не найдено - ошибка
                ErrorTable::addErrorToList(new SemanticError(
                    SemanticError::UndefinedVar(simpleExpr1->identifier->id, fieldName)
                ));
            }
            break;

        case _EXPRESSION_FIELD_ACCESS:
            if (simpleExpr1->simpleExpr && simpleExpr1->identifier) {
                auto receiverTypeOpt = inferSimpleExprType(simpleExpr1->simpleExpr);
                std::string fieldName = simpleExpr1->identifier->name;

                if (receiverTypeOpt.has_value()) {
                    if (receiverTypeOpt->kind == DataType::Kind::Class) {
                        auto classIt = ctx().classes.find(receiverTypeOpt->className);
                        if (classIt != ctx().classes.end()) {
                            auto fieldOpt = classIt->second->resolveField(fieldName, currentClass);
                            if (fieldOpt.has_value()) {
                                return fieldOpt.value()->dataType;
                            }
                            // Поле не найдено - ошибка
                            ErrorTable::addErrorToList(new SemanticError(
                                SemanticError::UndefinedVar(simpleExpr1->identifier->id, fieldName)
                            ));
                        }
                    } else if (receiverTypeOpt->kind == DataType::Kind::Array && fieldName == "length") {
                        return DataType::makeInt();
                    }
                }
            }
            break;

        case _METHOD_CALL:
            if (simpleExpr1->simpleExpr1 && simpleExpr1->argumentExprs) {
                auto argTypes = collectArgumentTypes(simpleExpr1->argumentExprs);
                SimpleExpr1Node* callee = simpleExpr1->simpleExpr1;

                if (callee->type == _EXPRESSION_FIELD_ACCESS && callee->identifier) {
                    std::string methodName = callee->identifier->name;
                    auto objTypeOpt = inferSimpleExprType(callee->simpleExpr);

                    if (objTypeOpt.has_value() && objTypeOpt->kind == DataType::Kind::Class) {
                        auto classIt = ctx().classes.find(objTypeOpt->className);
                        if (classIt != ctx().classes.end()) {
                            auto methodOpt = classIt->second->resolveMethod(methodName, argTypes, currentClass);
                            if (methodOpt.has_value()) {
                                for (auto* t : argTypes) delete t;
                                return methodOpt.value()->returnType;
                            } else {
                                std::string argsStr = "(";
                                for (size_t i = 0; i < argTypes.size(); ++i) {
                                    if (i > 0) argsStr += ", ";
                                    argsStr += argTypes[i]->toString();
                                }
                                argsStr += ")";
                                ErrorTable::addErrorToList(new SemanticError(
                                    SemanticError::MethodCandidateNotFound(
                                        simpleExpr1->id, methodName, argsStr)
                                ));
                            }
                        }
                    }
                } else if (callee->type == _IDENTIFIER && callee->identifier) {
                    std::string methodName = callee->identifier->name;
                    if (currentClass) {
                        auto methodOpt = currentClass->resolveMethod(methodName, argTypes, currentClass);
                        if (methodOpt.has_value()) {
                            for (auto* t : argTypes) delete t;
                            return methodOpt.value()->returnType;
                        } else {
                            std::string argsStr = "(";
                            for (size_t i = 0; i < argTypes.size(); ++i) {
                                if (i > 0) argsStr += ", ";
                                argsStr += argTypes[i]->toString();
                            }
                            argsStr += ")";
                            ErrorTable::addErrorToList(new SemanticError(
                                SemanticError::MethodCandidateNotFound(
                                    simpleExpr1->id, methodName, argsStr)
                            ));
                        }
                    }
                } else if (callee->type == _THIS_FIELD_ACCESS && callee->identifier) {
                    std::string methodName = callee->identifier->name;
                    if (currentClass) {
                        auto methodOpt = currentClass->resolveMethod(methodName, argTypes, currentClass);
                        if (methodOpt.has_value()) {
                            for (auto* t : argTypes) delete t;
                            return methodOpt.value()->returnType;
                        } else {
                            std::string argsStr = "(";
                            for (size_t i = 0; i < argTypes.size(); ++i) {
                                if (i > 0) argsStr += ", ";
                                argsStr += argTypes[i]->toString();
                            }
                            argsStr += ")";
                            ErrorTable::addErrorToList(new SemanticError(
                                SemanticError::MethodCandidateNotFound(
                                    simpleExpr1->id, methodName, argsStr)
                            ));
                        }
                    }
                } else if (callee->type == _SUPER_FIELD_ACCESS && callee->identifier) {
                    std::string methodName = callee->identifier->name;
                    if (currentClass && currentClass->parent) {
                        auto methodOpt = currentClass->parent->resolveMethod(methodName, argTypes, currentClass);
                        if (methodOpt.has_value()) {
                            for (auto* t : argTypes) delete t;
                            return methodOpt.value()->returnType;
                        } else {
                            std::string argsStr = "(";
                            for (size_t i = 0; i < argTypes.size(); ++i) {
                                if (i > 0) argsStr += ", ";
                                argsStr += argTypes[i]->toString();
                            }
                            argsStr += ")";
                            ErrorTable::addErrorToList(new SemanticError(
                                SemanticError::MethodCandidateNotFound(
                                    simpleExpr1->id, methodName, argsStr)
                            ));
                        }
                    }
                }

                for (auto* t : argTypes) delete t;
            }
            break;

        case _PLAIN_THIS:
            if (currentClass) {
                return DataType::makeClass(currentClass->name);
            }
            break;

        case _ARRAY_BUILDER:
        case _ARRAY_WITH_TYPE_BUILDER:
            if (simpleExpr1->simpleType) {
                DataType elemType = DataType::createFromNode(simpleExpr1->simpleType);
                return DataType::makeArray(elemType);
            } else if (simpleExpr1->argumentExprs) {
                auto argTypes = collectArgumentTypes(simpleExpr1->argumentExprs);
                if (!argTypes.empty()) {
                    DataType elemType = *argTypes[0];
                    for (auto* t : argTypes) delete t;
                    return DataType::makeArray(elemType);
                }
                for (auto* t : argTypes) delete t;
            }
            return DataType::makeArray(DataType(DataType::Kind::Any));

        case _PARENTHESIZED_EXPR:
            // (expr) - выражение в скобках, возвращаем тип внутреннего выражения
            if (simpleExpr1->expr) {
                return inferExprType(simpleExpr1->expr);
            }
            break;

        case _UNIT_LITERAL:
            // () - литерал Unit
            return DataType::makeUnit();

        default:
            break;
    }

    return std::nullopt;
}

// ================== Вспомогательные методы ==================

void TypeCheckVisitor::checkAssignmentCompatibility(const DataType& targetType,
                                                     const DataType& sourceType,
                                                     int line,
                                                     const std::string& context) {
    if (!sourceType.isAssignableTo(targetType)) {
        ErrorTable::addErrorToList(new SemanticError(
            SemanticError::TypeMismatch(line, targetType.toString(), sourceType.toString())
        ));
    }
}

void TypeCheckVisitor::checkNotVal(bool isVal, const std::string& varName, int line) {
    if (isVal && !currentMethod->isPrimaryConstructor) {
        ErrorTable::addErrorToList(new SemanticError(
            SemanticError::ValReassignment(line, varName)
        ));
    }
}

std::vector<DataType*> TypeCheckVisitor::collectArgumentTypes(ArgumentExprsNode* args) {
    std::vector<DataType*> result;

    if (args && args->exprs && args->exprs->exprs) {
        for (ExprNode* expr : *args->exprs->exprs) {
            auto typeOpt = inferExprType(expr);
            if (typeOpt.has_value()) {
                result.push_back(new DataType(typeOpt.value()));
            }
        }
    }

    return result;
}
