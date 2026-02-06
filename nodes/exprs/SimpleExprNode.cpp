#include "SimpleExprNode.h"
#include "../class/ConstrInvokeNode.h"
#include "../stats/BlockStatsNode.h"
#include "SimpleExpr1Node.h"
#include "ExprNode.h"
#include "ArgumentExprsNode.h"
#include "ExprsNode.h"
#include "../type/SimpleTypeNode.h"
#include "semantic/SemanticContext.h"
#include "semantic/tables/tables.hpp"
#include "semantic/error/ErrorTable.h"
#include "semantic/error/SemanticError.h"

SimpleExprNode::SimpleExprNode() {
    fullId = nullptr;
    arguments = nullptr;
    simpleType = nullptr;
    blockStats = nullptr;
    simpleExpr1 = nullptr;
}

SimpleExprNode *SimpleExprNode::createNewObjectNode(IdNode *fullId, ArgumentExprsNode *arguments) {
    SimpleExprNode* node = new SimpleExprNode();
    node->type = _INSTANCE_CREATING;
    node->fullId = fullId;
    node->arguments = arguments;
    return node;
}

SimpleExprNode *SimpleExprNode::createNewArrayNode(SimpleTypeNode *simpleType, ArgumentExprsNode *arguments) {
    SimpleExprNode* node = new SimpleExprNode();
    node->type = _ARRAY_CREATING;
    node->simpleType = simpleType;
    node->arguments = arguments;
    return node;
}

SimpleExprNode* SimpleExprNode::createBlockStatsNode(BlockStatsNode* blockStats) {
    SimpleExprNode* node = new SimpleExprNode();
    node->type = _BLOCK_STATS;
    node->blockStats = blockStats;
    return node;
}

SimpleExprNode* SimpleExprNode::createSimpleExpr1Node(SimpleExpr1Node* simpleExpr1) {
    SimpleExprNode* node = new SimpleExprNode();
    node->type = _SIMPLE_EXPR_1;
    node->simpleExpr1 = simpleExpr1;
    return node;
}

SimpleExprNode *SimpleExprNode::copy() {
    SimpleExprNode* node = new SimpleExprNode();
    node->type = type;

    if (fullId) {
        node->fullId = fullId->copy();
    }
    if (arguments) {
        node->arguments = arguments->copy();
    }
    if (simpleType) {
        node->simpleType = simpleType->copy();
    }
    if (blockStats) {
        node->blockStats = blockStats->copy();
    }
    if (simpleExpr1) {
        node->simpleExpr1 = simpleExpr1->copy();
    }
    return node;
}

string SimpleExprNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, fullId, "new object's type");
    addDotChild(dot, arguments, "arguments");
    addDotChild(dot, simpleType, "type of array");
    addDotChild(dot, blockStats, "block statements");
    addDotChild(dot, simpleExpr1, "simple expr 1");

    return dot;
}

string SimpleExprNode::getDotLabel() const {
    return simpleExprToString(type);
}

list<Node *> SimpleExprNode::getChildren() const {
    list<Node *> children = {};
    addChildIfNotNull(children, fullId);
    addChildIfNotNull(children, blockStats);
    addChildIfNotNull(children, simpleExpr1);
    addChildIfNotNull(children, arguments);
    addChildIfNotNull(children, simpleType);
    return children;
}

// Helper function to infer block stats type
static DataType inferBlockStatsType(
    BlockStatsNode* blockStats,
    ClassMetaInfo* currentClass,
    MethodMetaInfo* currentMethod,
    Scope* currentScope,
    int parentsConsider = PARENTS_CONSIDER
) {
    if (!blockStats || !blockStats->blockStats || blockStats->blockStats->empty()) {
        return DataType::makeUnit();
    }

    // Type of block is the type of the last statement
    BlockStatNode* lastStat = blockStats->blockStats->back();

    if (lastStat->expr) {
        return lastStat->expr->inferType(currentClass, currentMethod, currentScope, parentsConsider);
    }

    // If last statement is a variable definition, type is Unit
    return DataType::makeUnit();
}

DataType SimpleExprNode::inferType(
    ClassMetaInfo* currentClass,
    MethodMetaInfo* currentMethod,
    Scope* currentScope,
    int parentsConsider
) const {
    switch (type) {
        case _SIMPLE_EXPR_1:
            if (simpleExpr1) {
                return simpleExpr1->inferType(currentClass, currentMethod, currentScope, parentsConsider);
            }
            throw SemanticError::InternalError(id, "SimpleExprNode _SIMPLE_EXPR_1 without simpleExpr1");

        case _INSTANCE_CREATING:
            if (fullId) {
                std::string className = fullId->name;

                // Primitive types - synthetic constructors from transformLiterals
                auto primitiveType = DataType::primitiveFromName(className);
                if (primitiveType.has_value()) {
                    return primitiveType.value();
                }

                auto classIt = ctx().classes.find(className);
                if (classIt != ctx().classes.end()) {
                    ClassMetaInfo* classInfo = classIt->second;

                    if (classInfo->modifiers.hasModifier(_ABSTRACT)) {
                        throw SemanticError::AbstractClassInstantiated(id, className);
                    }

                    auto argTypes = arguments->getArgsTypes(currentClass, currentMethod, currentScope, parentsConsider);
                    auto constrOpt = classInfo->resolveMethod(className, argTypes, currentClass);

                    if (!constrOpt.has_value() && !argTypes.empty()) {
                        std::string argsStr = "(";
                        for (size_t i = 0; i < argTypes.size(); ++i) {
                            if (i > 0) argsStr += ", ";
                            argsStr += argTypes[i]->toString();
                        }
                        argsStr += ")";
                        for (auto* t : argTypes) delete t;
                        throw SemanticError::ConstructorNotFound(id, className + argsStr);
                    }

                    for (auto* t : argTypes) delete t;
                    return DataType::makeClass(className);
                }
                throw SemanticError::UndefinedClass(id, className);
            }
            throw SemanticError::InternalError(id, "SimpleExprNode _INSTANCE_CREATING without fullId");

        case _ARRAY_CREATING:
            if (simpleType) {
                DataType elementsType = DataType::createFromNode(simpleType);

                if (arguments) {
                    auto argTypes = arguments->getArgsTypes(currentClass, currentMethod, currentScope, parentsConsider);
                    for (size_t i = 0; i < argTypes.size(); ++i) {
                        if (!argTypes[i]->isAssignableTo(elementsType)) {
                            std::string argTypeStr = argTypes[i]->toString();
                            for (auto* t : argTypes) delete t;
                            throw SemanticError::TypeMismatch(id, elementsType.toString(), argTypeStr);
                        }
                    }
                    for (auto* t : argTypes) delete t;
                }

                return DataType::makeArray(elementsType);
            }
            throw SemanticError::InternalError(id, "SimpleExprNode _ARRAY_CREATING without simpleType");

        case _BLOCK_STATS:
            if (blockStats) {
                return inferBlockStatsType(blockStats, currentClass, currentMethod, currentScope, parentsConsider);
            }
            throw SemanticError::InternalError(id, "SimpleExprNode _BLOCK_STATS without blockStats");
    }

    throw SemanticError::InternalError(id, "Unknown SimpleExprNode type");
}

