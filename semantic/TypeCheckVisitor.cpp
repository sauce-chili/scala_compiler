//
// TypeCheckVisitor - проверяет типы выражений и присваиваний
//

#include "TypeCheckVisitor.h"

#include "nodes/nodes.h"
#include "nodes/func/FunDefNode.h"
#include "nodes/class/ClassDefNode.h"
#include "nodes/stats/BlockStatsNode.h"
#include "nodes/stats/BlockStatNode.h"
#include "nodes/var/VarDefsNode.h"
#include "nodes/exprs/ExprNode.h"
#include "nodes/exprs/AssignmentNode.h"
#include "semantic/error/ErrorTable.h"
#include "semantic/error/SemanticError.h"

void TypeCheckVisitor::visitTree(TopStatSeqNode *root) {
    if (root) visit(root);
}

void TypeCheckVisitor::visit(Node *node) {
    if (!node) return;

    if (auto *classDef = dynamic_cast<ClassDefNode *>(node)) {
        visitClassDef(classDef);
        return;
    }

    if (currentClass) {
        if (auto *funDef = dynamic_cast<FunDefNode *>(node)) {
            visitFunDef(funDef);
            return;
        }
    }

    if (currentMethod) {
        if (auto *blockStats = dynamic_cast<BlockStatsNode *>(node)) {
            visitBlockStats(blockStats);
            return;
        }

        if (auto *expr = dynamic_cast<ExprNode *>(node); expr && expr->enumerators) {
            visitForExpression(expr);
            return;
        }
    }

    visitChildren(node);
}

void TypeCheckVisitor::visitChildren(Node *node) {
    for (Node *child: node->getChildren()) {
        visit(child);
    }
}

void TypeCheckVisitor::visitClassDef(ClassDefNode *node) {
    if (!node->fullId) return;

    std::string className = node->fullId->name;
    auto it = ctx().classes.find(className);
    if (it == ctx().classes.end()) return;

    ClassMetaInfo *prevClass = currentClass;
    currentClass = it->second;

    if (node->classTemplateOpt) {
        visit(node->classTemplateOpt);
    }

    currentClass = prevClass;
}

void TypeCheckVisitor::visitFunDef(FunDefNode *node) {
    if (!currentClass) return;

    std::string methodName;
    std::vector<DataType *> argTypes;

    if (node->funSig && node->funSig->fullId) {
        methodName = node->funSig->fullId->name;

        if (node->funSig->params && node->funSig->params->funcParams) {
            for (auto *paramNode: *node->funSig->params->funcParams) {
                DataType *argType = new DataType(DataType::createFromNode(paramNode->simpleType));
                argTypes.push_back(argType);
            }
        }
    } else if (node->isConstructor() && node->funcParams) {
        methodName = currentClass->name;
        if (node->funcParams->funcParams) {
            for (auto *paramNode: *node->funcParams->funcParams) {
                DataType *argType = new DataType(DataType::createFromNode(paramNode->simpleType));
                argTypes.push_back(argType);
            }
        }
    } else {
        return;
    }

    auto methodOpt = currentClass->resolveMethod(methodName, argTypes, currentClass);

    for (const auto *t: argTypes) delete t;

    if (!methodOpt.has_value()) return;

    MethodMetaInfo *prevMethod = currentMethod;
    Scope *prevScope = currentScope;

    currentMethod = methodOpt.value();
    currentScope = nullptr;

    if (node->expr) visit(node->expr);
    if (node->constrExpr) visit(node->constrExpr);

    currentMethod = prevMethod;
    currentScope = prevScope;
}

void TypeCheckVisitor::visitBlockStats(BlockStatsNode *node) {
    if (!currentMethod) return;

    Scope *prevScope = currentScope;
    currentScope = node->blockScope;

    if (node->blockStats) {
        for (auto *blockStat: *node->blockStats) {
            visitBlockStat(blockStat);
        }
    }

    currentScope = prevScope;
}

void TypeCheckVisitor::visitBlockStat(BlockStatNode *node) {
    if (!node) return;

    if (node->varDefs) {
        visitVarDefs(node->varDefs);
    }

    if (node->expr) {
        visitExpr(node->expr);
    }
}

void TypeCheckVisitor::visitVarDefs(VarDefsNode *node) {
    if (!node || !currentMethod) return;

    if (node->expr && node->simpleType) {
        DataType declaredType = DataType::createFromNode(node->simpleType);
        try {
            DataType exprType = node->expr->inferType(currentClass, currentMethod, currentScope);
            int line = node->fullId ? node->fullId->id : 0;
            std::string varName = node->fullId ? node->fullId->name : "<unknown>";

            checkAssignmentCompatibility(declaredType, exprType, line,
                                         "Variable '" + varName + "' initialization");
        } catch (const SemanticError &err) {
            ErrorTable::addErrorToList(new SemanticError(err));
        }
    }

    if (node->expr) {
        visitExpr(node->expr);
    }
}

void TypeCheckVisitor::visitExpr(ExprNode *node) {
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
                try {
                    DataType condType = (*it)->inferType(currentClass, currentMethod, currentScope);
                    if (condType.kind != DataType::Kind::Bool) {
                        ErrorTable::addErrorToList(new SemanticError(
                            SemanticError::ConditionNotBoolean(node->id, condType.toString())
                        ));
                    }
                } catch (const SemanticError &err) {
                    ErrorTable::addErrorToList(new SemanticError(err));
                }
            }
            break;

        case _WHILE:
        case _DO_WHILE:
            if (node->exprs && !node->exprs->empty()) {
                auto it = node->exprs->begin();
                try {
                    DataType condType = (*it)->inferType(currentClass, currentMethod, currentScope);
                    if (condType.kind != DataType::Kind::Bool) {
                        ErrorTable::addErrorToList(new SemanticError(
                            SemanticError::ConditionNotBoolean(node->id, condType.toString())
                        ));
                    }
                } catch (const SemanticError &err) {
                    ErrorTable::addErrorToList(new SemanticError(err));
                }
            }
            break;

        case _INFIX:
            // для проверки корректности вызовов методы(существование метода и соответствие вызова сигнатуре метода)
            try {
                node->inferType(currentClass, currentMethod, currentScope);
            } catch (const SemanticError &err) {
                ErrorTable::addErrorToList(new SemanticError(err));
            }
            break;

        default:
            break;
    }

    visit(node);
}

void TypeCheckVisitor::checkAssignment(
        VarMetaInfo *fieldInfo,
        const std::string &varName,
        const DataType &exprType,
        int line) {

    if (fieldInfo->isInit || !currentMethod->isPrimaryConstructor) {
        checkNotVal(fieldInfo->isVal, varName, line);
    }
    checkAssignmentCompatibility(fieldInfo->dataType, exprType, line,
                                 "Assignment to field '" + varName + "'");
}

void TypeCheckVisitor::visitAssignment(AssignmentNode *node) {
    if (!node || !currentMethod) return;

    int line = node->fullId ? node->fullId->id : node->id;

    // 1. ID_ASSIGNMENT: x = value
    if (node->fullId && !node->simpleExpr && !node->simpleExpr1) {
        std::string varName = node->fullId->name;
        try {
            DataType exprType = node->expr->inferType(currentClass, currentMethod, currentScope);

            auto localOpt = currentMethod->resolveLocal(varName, currentScope);
            if (localOpt.has_value()) {
                MethodVarMetaInfo *varInfo = localOpt.value();
                checkAssignment(varInfo, varName, exprType, line);
            } else {
                auto fieldOpt = currentClass->resolveField(varName, currentClass);
                if (fieldOpt.has_value()) {
                    FieldMetaInfo *fieldInfo = fieldOpt.value();
                    checkAssignment(fieldInfo, varName, exprType, line);
                } else {
                    ErrorTable::addErrorToList(new SemanticError(
                        SemanticError::UndefinedVar(line, varName)
                    ));
                }
            }
        } catch (const SemanticError &err) {
            ErrorTable::addErrorToList(new SemanticError(err));
        }
    }
    // 2. FIELD_ASSIGNMENT: obj.field = value
    else if (node->simpleExpr && node->fullId) {
        std::string fieldName = node->fullId->name;
        try {
            DataType receiverType = node->simpleExpr->inferType(currentClass, currentMethod, currentScope);
            DataType exprType = node->expr->inferType(currentClass, currentMethod, currentScope);

            if (receiverType.kind == DataType::Kind::Class) {
                auto classIt = ctx().classes.find(receiverType.className);
                if (classIt != ctx().classes.end()) {
                    ClassMetaInfo *receiverClass = classIt->second;
                    auto fieldOpt = receiverClass->resolveField(fieldName, currentClass);

                    if (fieldOpt.has_value()) {
                        FieldMetaInfo *fieldInfo = fieldOpt.value();
                        checkAssignment(fieldInfo, fieldName, exprType, line);
                    } else {
                        ErrorTable::addErrorToList(new SemanticError(
                            SemanticError::UndefinedVar(line, fieldName)
                        ));
                    }
                }
            }
        } catch (const SemanticError &err) {
            ErrorTable::addErrorToList(new SemanticError(err));
        }
    }
    // 3. ARRAY_ASSIGNMENT: arr(index) = value
    else if (node->simpleExpr1 && node->argumentExprs) {
        try {
            DataType arrType = node->simpleExpr1->inferType(currentClass, currentMethod, currentScope);

            if (arrType.kind != DataType::Kind::Array) {
                ErrorTable::addErrorToList(new SemanticError(
                    SemanticError::ExprNotArray(line)
                ));
            } else {
                auto argTypes = node->argumentExprs->getArgsTypes(currentClass, currentMethod, currentScope);
                if (!argTypes.empty() && argTypes[0]->kind != DataType::Kind::Int) {
                    ErrorTable::addErrorToList(new SemanticError(
                        SemanticError::TypeMismatch(line, "Int", argTypes[0]->toString())
                    ));
                }

                DataType exprType = node->expr->inferType(currentClass, currentMethod, currentScope);
                if (arrType.elementType) {
                    checkAssignmentCompatibility(*arrType.elementType, exprType, line,
                                                 "Array element assignment");
                }

                for (auto *t: argTypes) delete t;
            }
        } catch (const SemanticError &err) {
            ErrorTable::addErrorToList(new SemanticError(err));
        }
    }

    currentMethod->executeAssign(node, currentScope);
}

void TypeCheckVisitor::visitForExpression(ExprNode *node) {
    if (!node || !node->enumerators || !currentMethod) return;

    Scope *prevScope = currentScope;
    currentScope = node->enumerators->scope;

    if (node->enumerators->generator && node->enumerators->generator->expr) {
        try {
            // Выводит тип элементов (если есть элементы несовместимых типов - кидается ошибка)
            DataType iterableType = node->enumerators->generator->expr->inferType(
                currentClass, currentMethod, currentScope);
            if (iterableType.kind != DataType::Kind::Array) {
                ErrorTable::addErrorToList(new SemanticError(
                    SemanticError::ForLoopNotArray(node->id, iterableType.toString())
                ));
            }
            DataType* arrayElementType = iterableType.elementType;
            DataType generatorVarDeclaredType = DataType::createFromNode(node->enumerators->generator->simpleType);
            if (!arrayElementType->isAssignableTo(generatorVarDeclaredType)) {
                ErrorTable::addErrorToList(new SemanticError(
                        SemanticError::TypeMismatch(node->id, generatorVarDeclaredType.toString(), arrayElementType->toString())
                ));
            }
        } catch (const SemanticError &err) {
            ErrorTable::addErrorToList(new SemanticError(err));
        }
    }

    if (node->exprs) {
        for (auto *expr: *node->exprs) {
            visit(expr);
        }
    }

    currentScope = prevScope;
}

// ================== Вспомогательные методы ==================

void TypeCheckVisitor::checkAssignmentCompatibility(const DataType &targetType,
                                                    const DataType &sourceType,
                                                    int line,
                                                    const std::string &context) {
    if (!sourceType.isAssignableTo(targetType)) {
        ErrorTable::addErrorToList(new SemanticError(
            SemanticError::TypeMismatch(line, targetType.toString(), sourceType.toString())
        ));
    }
}

void TypeCheckVisitor::checkNotVal(bool isVal, const std::string& varName, int line) {
    if (isVal) {
        ErrorTable::addErrorToList(new SemanticError(
            SemanticError::ValReassignment(line, varName)
        ));
    }
}
