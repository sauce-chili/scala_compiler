//
// TypeExistenceValidator - проверяет существование всех используемых типов
//

#include "TypeExistenceValidator.h"

#include "nodes/nodes.h"
#include "nodes/func/FunDefNode.h"
#include "nodes/func/FuncParamNode.h"
#include "nodes/func/FuncParamsNode.h"
#include "nodes/class/ClassDefNode.h"
#include "nodes/class/ClassParamNode.h"
#include "nodes/class/ClassParamsNode.h"
#include "nodes/var/VarDefsNode.h"
#include "nodes/type/SimpleTypeNode.h"
#include "nodes/exprs/SimpleExprNode.h"
#include "semantic/tables/tables.hpp"
#include "semantic/error/ErrorTable.h"
#include "semantic/error/SemanticError.h"

void TypeExistenceValidator::visitTree(TopStatSeqNode* root) {
    if (root) visit(root);
}

void TypeExistenceValidator::visit(Node* node) {
    if (!node) return;

    // Класс
    if (auto* classDef = dynamic_cast<ClassDefNode*>(node)) {
        visitClassDef(classDef);
        return;
    }

    // Метод
    if (auto* funDef = dynamic_cast<FunDefNode*>(node)) {
        visitFunDef(funDef);
        return;
    }

    // Определение переменной
    if (auto* varDefs = dynamic_cast<VarDefsNode*>(node)) {
        visitVarDefs(varDefs);
        return;
    }

    // SimpleExpr (для new ClassName)
    if (auto* simpleExpr = dynamic_cast<SimpleExprNode*>(node)) {
        visitSimpleExpr(simpleExpr);
        return;
    }

    // По умолчанию: идем вглубь
    visitChildren(node);
}

void TypeExistenceValidator::visitChildren(Node* node) {
    for (Node* child : node->getChildren()) {
        visit(child);
    }
}

void TypeExistenceValidator::visitClassDef(ClassDefNode* node) {
    // Проверяем типы параметров конструктора класса
    if (node->classParams && node->classParams->classParams) {
        for (auto* classParam : *node->classParams->classParams) {
            visitClassParam(classParam);
        }
    }

    // Продолжаем обход тела класса
    visitChildren(node);
}

void TypeExistenceValidator::visitFunDef(FunDefNode* node) {
    // Проверяем возвращаемый тип
    if (node->simpleType) {
        validateType(node->simpleType);
    }

    // Проверяем типы параметров
    if (node->funSig && node->funSig->params && node->funSig->params->funcParams) {
        for (auto* param : *node->funSig->params->funcParams) {
            visitFuncParam(param);
        }
    }

    // Для вторичных конструкторов
    if (node->funcParams && node->funcParams->funcParams) {
        for (auto* param : *node->funcParams->funcParams) {
            visitFuncParam(param);
        }
    }

    // Продолжаем обход тела метода
    visitChildren(node);
}

void TypeExistenceValidator::visitVarDefs(VarDefsNode* node) {
    // Проверяем объявленный тип переменной
    if (node->simpleType) {
        validateType(node->simpleType);
    }

    // Продолжаем обход (инициализирующее выражение)
    visitChildren(node);
}

void TypeExistenceValidator::visitSimpleExpr(SimpleExprNode* node) {
    // Проверяем создание экземпляра: new ClassName(args)
    if (node->type == _INSTANCE_CREATING && node->fullId) {
        validateClassName(node->fullId->name, node->fullId->id);
    }

    // Проверяем создание массива: new Array[T](size)
    if (node->type == _ARRAY_CREATING && node->simpleType) {
        validateType(node->simpleType);
    }

    // Продолжаем обход
    visitChildren(node);
}

void TypeExistenceValidator::visitFuncParam(FuncParamNode* node) {
    if (node && node->simpleType) {
        validateType(node->simpleType);
    }
}

void TypeExistenceValidator::visitClassParam(ClassParamNode* node) {
    if (node && node->simpleType) {
        validateType(node->simpleType);
    }
}

void TypeExistenceValidator::validateType(SimpleTypeNode* typeNode) {
    if (!typeNode) return;

    // Массив Array[T] - рекурсивно проверяем тип элемента
    if (typeNode->arrayElemType) {
        validateType(typeNode->arrayElemType);
        return;
    }

    // Именованный тип
    if (typeNode->fullId) {
        std::string typeName = typeNode->fullId->name;
        int line = typeNode->fullId->id;

        // Примитивы всегда валидны
        if (typeName == "Int" || typeName == "Double" || typeName == "Boolean" ||
            typeName == "String" || typeName == "Char" || typeName == "Unit" ||
            typeName == "Any") {
            return;
        }

        // Проверяем наличие класса в контексте
        if (ctx().classes.find(typeName) == ctx().classes.end()) {
            ErrorTable::addErrorToList(new SemanticError(
                SemanticError::UndeclaredType(line, typeName)
            ));
        }
    }
}

void TypeExistenceValidator::validateClassName(const std::string& className, int line) {
    // Проверяем наличие класса в контексте
    if (ctx().classes.find(className) == ctx().classes.end()) {
        ErrorTable::addErrorToList(new SemanticError(
            SemanticError::UndeclaredType(line, className)
        ));
    }
}
