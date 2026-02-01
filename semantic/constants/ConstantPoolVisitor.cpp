#include "ConstantPoolVisitor.h"
#include "ConstantPoolBuilder.h"

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
#include "nodes/id/IdNode.h"

#include "semantic/SemanticContext.h"
#include "semantic/tables/tables.hpp"
#include "semantic/scopes/Scope.h"

// ==================== Точка входа ====================

void ConstantPoolVisitor::visitTree(TopStatSeqNode* root) {
    if (root) visit(root);
}

// ==================== Обход узлов ====================

void ConstantPoolVisitor::visit(Node* node) {
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

        if (auto* expr = dynamic_cast<ExprNode*>(node)) {
            visitExpr(expr);
            return;
        }

        if (auto* simpleExpr1 = dynamic_cast<SimpleExpr1Node*>(node)) {
            visitSimpleExpr1(simpleExpr1);
            return;
        }
    }

    visitChildren(node);
}

void ConstantPoolVisitor::visitChildren(Node* node) {
    for (Node* child : node->getChildren()) {
        visit(child);
    }
}

// ==================== Класс ====================

void ConstantPoolVisitor::visitClassDef(ClassDefNode* node) {
    if (!node->fullId) return;

    std::string className = node->fullId->name;
    auto it = ctx().classes.find(className);
    if (it == ctx().classes.end()) return;

    ClassMetaInfo* prevClass = currentClass;
    currentClass = it->second;

    // Создаём ConstantPoolBuilder для класса, если его нет
    if (!currentClass->constantPool) {
        currentClass->constantPool = new ConstantPoolBuilder();
    }

    // Добавляем константу для имени класса
    currentClass->constantPool->addClass(currentClass->jvmName);

    // Добавляем константы для родительского класса
    if (currentClass->parent) {
        currentClass->constantPool->addClass(currentClass->parent->jvmName);
    }

    // Добавляем константы для всех полей
    for (auto& [fieldName, field] : currentClass->fields) {
        currentClass->constantPool->addFieldRef(currentClass, field);
    }

    // Добавляем константы для всех методов
    for (auto& [methodName, methodList] : currentClass->methods) {
        for (auto* method : methodList) {
            currentClass->constantPool->addMethodRef(currentClass, method);
        }
    }

    // Обходим тело класса для сбора литералов и внешних ссылок
    if (node->classTemplateOpt) {
        visit(node->classTemplateOpt);
    }

    currentClass = prevClass;
}

// ==================== Метод ====================

void ConstantPoolVisitor::visitFunDef(FunDefNode* node) {
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

    if (node->expr) visitExpr(node->expr);
    if (node->constrExpr) visit(node->constrExpr);

    currentMethod = prevMethod;
    currentScope = prevScope;
}

// ==================== Блоки ====================

void ConstantPoolVisitor::visitBlockStats(BlockStatsNode* node) {
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

void ConstantPoolVisitor::visitBlockStat(BlockStatNode* node) {
    if (!node) return;

    if (node->varDefs) {
        visitVarDefs(node->varDefs);
    }

    if (node->expr) {
        visitExpr(node->expr);
    }
}

void ConstantPoolVisitor::visitVarDefs(VarDefsNode* node) {
    if (!node) return;

    if (node->expr) {
        visitExpr(node->expr);
    }
}

// ==================== Выражения ====================

void ConstantPoolVisitor::visitExpr(ExprNode* node) {
    if (!node) return;

    switch (node->type) {
        case _INFIX:
            if (node->infixExpr) {
                visitInfixExpr(node->infixExpr);
            }
            break;

        case _ASSIGNMENT:
            if (node->assignment) {
                visitAssignment(node->assignment);
            }
            break;

        case _IF:
        case _IF_ELSE:
        case _WHILE:
        case _DO_WHILE:
        case _FOR_WITHOUT_YIELD:
        case _FOR_WITH_YIELD:
        case _RETURN_EXPR:
            // Обходим вложенные выражения
            if (node->exprs) {
                for (auto* expr : *node->exprs) {
                    visitExpr(expr);
                }
            }
            if (node->infixExpr) {
                visitInfixExpr(node->infixExpr);
            }
            break;

        default:
            if (node->exprs) {
                for (auto* expr : *node->exprs) {
                    visitExpr(expr);
                }
            }
            break;
    }
}

void ConstantPoolVisitor::visitInfixExpr(InfixExprNode* node) {
    if (!node) return;

    if (node->prefixExpr) {
        visitPrefixExpr(node->prefixExpr);
    }

    if (node->left) {
        visitInfixExpr(node->left);
    }

    if (node->right) {
        visitInfixExpr(node->right);
    }
}

void ConstantPoolVisitor::visitPrefixExpr(PrefixExprNode* node) {
    if (!node) return;

    if (node->simpleExpr) {
        visitSimpleExpr(node->simpleExpr);
    }
}

void ConstantPoolVisitor::visitSimpleExpr(SimpleExprNode* node) {
    if (!node) return;

    switch (node->type) {
        case _INSTANCE_CREATING:
            // new ClassName(args) - добавляем ссылку на класс и конструктор
            if (node->fullId) {
                std::string className = node->fullId->name;
                auto classIt = ctx().classes.find(className);
                if (classIt != ctx().classes.end()) {
                    ClassMetaInfo* targetClass = classIt->second;
                    // Добавляем ссылку на класс
                    currentClass->constantPool->addClass(targetClass->jvmName);

                    // Добавляем ссылку на конструктор
                    std::vector<DataType*> argTypes;
                    if (node->arguments && node->arguments->exprs) {
                        argTypes = node->arguments->getArgsTypes(currentClass, currentMethod, currentScope);
                    }

                    // Ищем конструктор (метод с именем класса)
                    auto ctorOpt = targetClass->resolveMethod(className, argTypes, currentClass);
                    if (ctorOpt.has_value()) {
                        currentClass->constantPool->addMethodRef(targetClass, ctorOpt.value());
                    }

                    for (auto* t : argTypes) delete t;
                }
            }
            // Обходим аргументы конструктора
            if (node->arguments && node->arguments->exprs) {
                for (auto* expr : *node->arguments->exprs->exprs) {
                    visitExpr(expr);
                }
            }
            break;

        case _ARRAY_CREATING:
            // new Array[T](size)
            if (node->arguments && node->arguments->exprs) {
                for (auto* expr : *node->arguments->exprs->exprs) {
                    visitExpr(expr);
                }
            }
            break;

        case _BLOCK_STATS:
            if (node->blockStats) {
                visitBlockStats(node->blockStats);
            }
            break;

        case _SIMPLE_EXPR_1:
            if (node->simpleExpr1) {
                visitSimpleExpr1(node->simpleExpr1);
            }
            break;

        default:
            break;
    }
}

void ConstantPoolVisitor::visitSimpleExpr1(SimpleExpr1Node* node) {
    if (!node || !currentClass || !currentClass->constantPool) return;

    switch (node->type) {
        // Литералы
        case _INTEGER_LITERAL:
        case _DOUBLE_LITERAL:
        case _STRING_LITERAL:
        case _CHAR_LITERAL:
        case _BOOL_LITERAL:
            addLiteralConstant(node);
            break;

        // Вызов метода: expr(args) или obj.method(args)
        case _METHOD_CALL:
            collectMethodCallRefs(node);
            // Обходим аргументы
            if (node->argumentExprs && node->argumentExprs->exprs) {
                for (auto* expr : *node->argumentExprs->exprs->exprs) {
                    visitExpr(expr);
                }
            }
            break;

        // Доступ к полю через выражение: expr.field
        case _EXPRESSION_FIELD_ACCESS:
            collectFieldAccessRefs(node);
            if (node->simpleExpr) {
                visitSimpleExpr(node->simpleExpr);
            }
            break;

        // this.field
        case _THIS_FIELD_ACCESS:
            if (node->identifier && currentClass) {
                std::string fieldName = node->identifier->name;
                auto fieldOpt = currentClass->resolveField(fieldName, currentClass);
                if (fieldOpt.has_value()) {
                    currentClass->constantPool->addFieldRef(currentClass, fieldOpt.value());
                }
            }
            break;

        // super.field
        case _SUPER_FIELD_ACCESS:
            if (node->identifier && currentClass && currentClass->parent) {
                std::string fieldName = node->identifier->name;
                auto fieldOpt = currentClass->parent->resolveField(fieldName, currentClass);
                if (fieldOpt.has_value()) {
                    currentClass->constantPool->addFieldRef(currentClass->parent, fieldOpt.value());
                }
            }
            break;

        // Array(elements) или Array[T](elements)
        case _ARRAY_BUILDER:
        case _ARRAY_WITH_TYPE_BUILDER:
            if (node->argumentExprs && node->argumentExprs->exprs) {
                for (auto* expr : *node->argumentExprs->exprs->exprs) {
                    visitExpr(expr);
                }
            }
            break;

        // Выражение в скобках
        case _PARENTHESIZED_EXPR:
            if (node->expr) {
                visitExpr(node->expr);
            }
            break;

        // Идентификатор (переменная или поле)
        case _IDENTIFIER:
            // Может быть локальной переменной или полем текущего класса
            if (node->identifier && currentClass) {
                std::string name = node->identifier->name;
                // Сначала проверяем локальные переменные
                if (currentMethod) {
                    auto localOpt = currentMethod->resolveLocal(name, currentScope);
                    if (localOpt.has_value()) {
                        break; // Локальная переменная, не добавляем в constant pool
                    }
                }
                // Проверяем поля класса
                auto fieldOpt = currentClass->resolveField(name, currentClass);
                if (fieldOpt.has_value()) {
                    currentClass->constantPool->addFieldRef(currentClass, fieldOpt.value());
                }
            }
            break;

        default:
            break;
    }
}

void ConstantPoolVisitor::visitAssignment(AssignmentNode* node) {
    if (!node) return;

    if (node->expr) {
        visitExpr(node->expr);
    }

    if (node->simpleExpr) {
        visitSimpleExpr(node->simpleExpr);
    }

    if (node->simpleExpr1) {
        visitSimpleExpr1(node->simpleExpr1);
    }
}

// ==================== Сбор внешних ссылок ====================

void ConstantPoolVisitor::collectMethodCallRefs(SimpleExpr1Node* node) {
    if (!node || !node->simpleExpr1 || !currentClass || !currentClass->constantPool) return;

    SimpleExpr1Node* callee = node->simpleExpr1;

    // Собираем типы аргументов для поиска метода
    std::vector<DataType*> argTypes;
    if (node->argumentExprs && node->argumentExprs->exprs) {
        argTypes = node->argumentExprs->getArgsTypes(currentClass, currentMethod, currentScope);
    }

    if (callee->type == _EXPRESSION_FIELD_ACCESS && callee->identifier && callee->simpleExpr) {
        // obj.method(args)
        std::string methodName = callee->identifier->name;

        try {
            DataType objType = callee->simpleExpr->inferType(currentClass, currentMethod, currentScope);
            std::string className = objType.getClassName();

            auto classIt = ctx().classes.find(className);
            if (classIt != ctx().classes.end()) {
                ClassMetaInfo* targetClass = classIt->second;

                // Добавляем ссылку на класс
                currentClass->constantPool->addClass(targetClass->jvmName);

                // Сначала проверяем, не является ли это доступом к массиву
                auto fieldOpt = targetClass->resolveField(methodName, currentClass);
                if (fieldOpt.has_value() && fieldOpt.value()->dataType.kind == DataType::Kind::Array) {
                    // Это доступ к элементу массива, не метод
                    currentClass->constantPool->addFieldRef(targetClass, fieldOpt.value());
                } else {
                    // Ищем метод
                    auto methodOpt = targetClass->resolveMethod(methodName, argTypes, currentClass);
                    if (methodOpt.has_value()) {
                        currentClass->constantPool->addMethodRef(targetClass, methodOpt.value());
                    }
                }
            }
        } catch (...) {
            // Игнорируем ошибки при определении типа
        }

        // Рекурсивно обходим receiver
        visitSimpleExpr(callee->simpleExpr);

    } else if (callee->type == _THIS_FIELD_ACCESS && callee->identifier) {
        // this.method(args)
        std::string methodName = callee->identifier->name;

        auto methodOpt = currentClass->resolveMethod(methodName, argTypes, currentClass);
        if (methodOpt.has_value()) {
            currentClass->constantPool->addMethodRef(currentClass, methodOpt.value());
        }

    } else if (callee->type == _SUPER_FIELD_ACCESS && callee->identifier) {
        // super.method(args)
        std::string methodName = callee->identifier->name;

        if (currentClass->parent) {
            currentClass->constantPool->addClass(currentClass->parent->jvmName);

            auto methodOpt = currentClass->parent->resolveMethod(methodName, argTypes, currentClass);
            if (methodOpt.has_value()) {
                currentClass->constantPool->addMethodRef(currentClass->parent, methodOpt.value());
            }
        }

    } else if (callee->type == _IDENTIFIER && callee->identifier) {
        // method(args) - вызов метода текущего класса или локальной функции
        std::string methodName = callee->identifier->name;

        auto methodOpt = currentClass->resolveMethod(methodName, argTypes, currentClass);
        if (methodOpt.has_value()) {
            currentClass->constantPool->addMethodRef(currentClass, methodOpt.value());
        }

    } else if (callee->type == _METHOD_CALL) {
        // Цепочка вызовов: a.b().c() или a(0)(1)
        // Рекурсивно обрабатываем
        visitSimpleExpr1(callee);
    }

    for (auto* t : argTypes) delete t;
}

void ConstantPoolVisitor::collectFieldAccessRefs(SimpleExpr1Node* node) {
    if (!node || !node->simpleExpr || !node->identifier || !currentClass || !currentClass->constantPool) return;

    std::string fieldName = node->identifier->name;

    try {
        DataType objType = node->simpleExpr->inferType(currentClass, currentMethod, currentScope);
        std::string className = objType.getClassName();

        auto classIt = ctx().classes.find(className);
        if (classIt != ctx().classes.end()) {
            ClassMetaInfo* targetClass = classIt->second;

            // Добавляем ссылку на класс
            currentClass->constantPool->addClass(targetClass->jvmName);

            // Добавляем ссылку на поле
            auto fieldOpt = targetClass->resolveField(fieldName, currentClass);
            if (fieldOpt.has_value()) {
                currentClass->constantPool->addFieldRef(targetClass, fieldOpt.value());
            }
        }
    } catch (...) {
        // Игнорируем ошибки при определении типа
    }
}

// ==================== Добавление констант ====================

void ConstantPoolVisitor::addLiteralConstant(SimpleExpr1Node* literal) {
    if (!literal || !currentClass || !currentClass->constantPool) return;

    ConstantPoolBuilder* pool = currentClass->constantPool;

    switch (literal->type) {
        case _INTEGER_LITERAL:
            pool->addInt(literal->intValue);
            break;

        case _DOUBLE_LITERAL:
            // Double реализуется через Float
            pool->addFloat(static_cast<float>(literal->doubleValue));
            break;

        case _STRING_LITERAL:
            pool->addString(literal->stringValue);
            break;

        case _CHAR_LITERAL:
            // Char как int
            pool->addInt(static_cast<int32_t>(literal->charValue));
            break;

        case _BOOL_LITERAL:
            // Bool как int (0 или 1)
            pool->addInt(literal->boolValue ? 1 : 0);
            break;

        default:
            break;
    }
}

void ConstantPoolVisitor::addMethodRefConstant(ClassMetaInfo* targetClass, MethodMetaInfo* method) {
    if (!targetClass || !method || !currentClass || !currentClass->constantPool) return;
    currentClass->constantPool->addMethodRef(targetClass, method);
}

void ConstantPoolVisitor::addFieldRefConstant(ClassMetaInfo* targetClass, FieldMetaInfo* field) {
    if (!targetClass || !field || !currentClass || !currentClass->constantPool) return;
    currentClass->constantPool->addFieldRef(targetClass, field);
}
