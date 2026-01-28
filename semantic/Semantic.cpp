#include "nodes/stats/TopStatSeqNode.h"
#include "nodes/class/ClassDefNode.h"
#include "nodes/definitions/DclNode.h"
#include "nodes/definitions/DefNode.h"
#include "nodes/stats/TopStatNode.h"
#include "nodes/class/ClassParamNode.h"
#include "nodes/class/ClassParamsNode.h"
#include "semantic/error/SemanticError.h"
#include "nodes/exprs/InfixExprNode.h"
#include "nodes/exprs/ExprNode.h"
#include "nodes/exprs/SimpleExpr1Node.h"

#include <iostream>
#include <stdexcept>

void normalizeInfixes(Node* node);
void transformInfixes(Node* node);

void TopStatSeqNode::convertAst() {
    for (TopStatNode* tsn: *(topStats)) {
        if (!tsn) continue;
        if (!tsn->tmplDef) continue;
        if (!tsn->tmplDef->classDef) continue;

        tsn->tmplDef->classDef->normalizeBody();
        tsn->tmplDef->classDef->validatePrimaryConstructorModifiers();
        tsn->tmplDef->validateModifiers();
        tsn->toFieldsFromPrimaryConstructor();
        tsn->initializeBaseConstructorFromFields();
        tsn->secondaryConstructorsToMethods();
        normalizeInfixes(tsn);
        transformInfixes(tsn);
    }
}

void ClassDefNode::normalizeBody() const {
    if (classTemplateOpt && classTemplateOpt->extensionPartClassTemplate && classTemplateOpt->extensionPartClassTemplate->templateStats) {
        classTemplateOpt->templateStats = classTemplateOpt->extensionPartClassTemplate->templateStats;
        classTemplateOpt->extensionPartClassTemplate->templateStats = nullptr;
    }
}

void TopStatNode::toFieldsFromPrimaryConstructor() {
    if (!tmplDef) return;
    if (!tmplDef->classDef) return;

    ClassDefNode *currentClass = tmplDef->classDef;

    if (!currentClass->classParams) return;
    if (!currentClass->classParams->classParams) return;
    if (currentClass->classParams->classParams->empty()) return;

    if (!currentClass->classTemplateOpt) throw std::runtime_error("No class template");

    TemplateStatsNode* classBody = currentClass->classTemplateOpt->templateStats;
    if (!classBody) {
        classBody = new TemplateStatsNode();
    }

    for (ClassParamNode* p: *(currentClass->classParams->classParams)) {
        if (!p) continue;
        if (classBody->containsVar(p->fullId->name)) {
            throw SemanticError::VarRedefinition(0, p->fullId->name);
        }
        //if (currentClass->classTemplateOpt->classParents->constr->arguments->contains()) {} // аргументы могут содержать вырадения по типу x + y

        TemplateStatNode *stat = new TemplateStatNode();
        IdNode *id = p->fullId->copy();
        if (p->type == ClassParamType::_VAR_CLASS_PARAM) {
            stat->dcl = DclNode::createVarDcl(id, p->simpleType->copy());
        } else {
            stat->dcl = DclNode::createValDcl(id, p->simpleType->copy());
        }
        stat->dcl->modifiers = p->modifiers->copy();

        currentClass->classTemplateOpt->templateStats->templateStats->push_front(stat);
    }

    currentClass->classParams = nullptr;
}

void TopStatNode::initializeBaseConstructorFromFields() const {
    if (!tmplDef) return;
    if (!tmplDef->classDef) return;

    ClassDefNode *currentClass = tmplDef->classDef;

    if (!currentClass->classTemplateOpt) return;
    if (!currentClass->classTemplateOpt->templateStats) return;

    if (!currentClass->classParams) {
        currentClass->classParams = new ClassParamsNode();
    }

    // Сущность конструктора
    TemplateStatNode *baseConstructor = new TemplateStatNode();
    // Вызов родительского конструктора (будет всегда, как минимум Object)
    BlockStatNode *superCall;
    SimpleExpr1Node* superConstructorName = SimpleExpr1Node::createIdNode(IdNode::createId("super"));
    if (currentClass->classTemplateOpt->extensionPartClassTemplate && currentClass->classTemplateOpt->extensionPartClassTemplate->argumentExprs) {
        superCall = BlockStatNode::createSimpleExpr1(SimpleExpr1Node::createMethodCallNode(
                superConstructorName, currentClass->classTemplateOpt->extensionPartClassTemplate->argumentExprs->copy())
        );

        // Убираем скобки с аргументами конструктора родителя
        currentClass->classTemplateOpt->extensionPartClassTemplate->fullId = currentClass->classTemplateOpt->extensionPartClassTemplate->fullId;
        currentClass->classTemplateOpt->extensionPartClassTemplate->argumentExprs = nullptr;
    } else {
        superCall = BlockStatNode::createSimpleExpr1(SimpleExpr1Node::createMethodCallNode(
                superConstructorName, nullptr)
        );
    }
    // Поля класса
    BlockStatsNode *blockStats = BlockStatsNode::addBlockStatToList(nullptr, nullptr);
    BlockStatsNode::addBlockStatToList(blockStats, superCall);
    for (TemplateStatNode* p: *(currentClass->classTemplateOpt->templateStats->templateStats)) {
        if (!p) continue;
        if (p->dcl) continue;
        if (!p->def->varDefs) continue;

        BlockStatNode *stat;
        SimpleTypeNode* typeOfVars = p->def->varDefs->simpleType ? p->def->varDefs->simpleType->copy() : nullptr;
        if (p->def->varDefs->type == StatType::_VAR_DECL) {
            stat = BlockStatNode::createVarDefsNode(
                    VarDefsNode::createVar(p->def->varDefs->fullId->copy(), typeOfVars, p->def->varDefs->expr->copy()));
        } else {
            stat = BlockStatNode::createVarDefsNode(
                    VarDefsNode::createVal(p->def->varDefs->fullId->copy(), typeOfVars, p->def->varDefs->expr->copy()));
        }
        p->def->varDefs->expr = nullptr;
        BlockStatsNode::addBlockStatToList(blockStats, stat);
    }

    FunSigNode *primaryConstrSignature = FunSigNode::createFunSig(IdNode::createId("this"), new FuncParamsNode(currentClass->classParams->copy()));
    // Конструкторы не имеют явного типа возврата в Scala
    FunDefNode *primaryConstructorNode = FunDefNode::createFunSigFunDef(
            primaryConstrSignature,
            nullptr,
            BlockStatNode::createExpr(SimpleExprNode::createBlockStatsNode(blockStats))
    );
    primaryConstructorNode->primaryConstructor = true;
    currentClass->classParams = new ClassParamsNode();
    baseConstructor->def = DefNode::createFunDef(primaryConstructorNode);
    currentClass->classTemplateOpt->templateStats->templateStats->push_front(baseConstructor);
}

void TemplateDefNode::validateModifiers() const {
    validateClassModifiers();
    if (classDef && classDef->classTemplateOpt && classDef->classTemplateOpt->templateStats) {
        classDef->classTemplateOpt->templateStats->validateModifiers();
    }
}

void TemplateDefNode::validateClassModifiers() const {
    string prevAccess;
    string prevInherit;
    for (ModifierNode* m: *this->getModifiers()->modifiers) {
        if (m->isAccessModifier()) {
            if (!prevAccess.empty()) {
                throw SemanticError::InvalidCombinationOfModifiers(0, prevAccess + " " + modifierToString(m->type));
            }
            prevAccess = modifierToString(m->type);
        } else if (m->isInheritModifier()) {
            if (!prevInherit.empty()) {
                throw SemanticError::InvalidCombinationOfModifiers(0, prevInherit + " " + modifierToString(m->type));
            }
            prevInherit = modifierToString(m->type);
        } else if (m->isOverrideModifier()) {
            throw SemanticError::InvalidCombinationOfModifiers(0, modifierToString(m->type));
        }
    }
}

void TemplateStatsNode::validateModifiers() const {
    for (TemplateStatNode* ts: *templateStats) {
        if (ts) {
            ts->validateModifiers();
        }
    }
}

void TemplateStatNode::validateModifiers() const {
    validateVarModifiers();
    validateMethodModifiers();
    validateSecondaryConstructorModifiers();
}

void TemplateStatNode::validateVarModifiers() const {
    ModifiersNode* modifiers;
    if (dcl && (dcl->type == _VAL_DECL || dcl->type == _VAR_DECL)) {
        modifiers = dcl->modifiers;
    } else if (def && def->varDefs) {
        modifiers = def->modifiers;
    } else {
        return;
    }

    string prevAccess;
    string prevInherit;
    bool overrided = false;
    for (ModifierNode* m: *modifiers->modifiers) {
        if (m->isAccessModifier()) {
            if (!prevAccess.empty()) {
                throw SemanticError::InvalidCombinationOfModifiers(0, prevAccess + " " + modifierToString(m->type));
            }
            prevAccess = modifierToString(m->type);
        } else if (m->isInheritModifier()) {
            if (m->type == _SEALED) {
                throw SemanticError::InvalidCombinationOfModifiers(0, modifierToString(m->type));
            }
            if (!prevInherit.empty()) {
                throw SemanticError::InvalidCombinationOfModifiers(0, prevInherit + " " + modifierToString(m->type));
            }
            prevInherit = modifierToString(m->type);
        } else if (m->isOverrideModifier()) {
            if (dcl && dcl->type == _VAR_DECL) {
                throw SemanticError::InvalidCombinationOfModifiers(0, modifierToString(m->type) + " to var");
            }

            if (overrided) {
                throw SemanticError::InvalidCombinationOfModifiers(0, modifierToString(m->type) + " " + modifierToString(m->type));
            }
            overrided = true;
        }
    }
}

void TemplateStatNode::validateMethodModifiers() const {
    ModifiersNode* modifiers;
    if (dcl && dcl->funSig) {
        modifiers = dcl->modifiers;
    } else if (def && def->funDef && def->funDef->funSig) {
        modifiers = def->modifiers;
    } else {
        return;
    }

    string prevAccess;
    string prevInherit;
    bool overrided = false;
    for (ModifierNode* m: *modifiers->modifiers) {
        if (m->isAccessModifier()) {
            if (!prevAccess.empty()) {
                throw SemanticError::InvalidCombinationOfModifiers(0, prevAccess + " " + modifierToString(m->type));
            }
            prevAccess = modifierToString(m->type);
        } else if (m->isInheritModifier()) {
            if (m->type == _SEALED) {
                throw SemanticError::InvalidCombinationOfModifiers(0, "values cannot be sealed");
            }
            if (m->type == _ABSTRACT) {
                throw SemanticError::InvalidCombinationOfModifiers(0, "abstract modifier can be used only for classes; it should be omitted for abstract members");
            }
            if (!prevInherit.empty()) {
                throw SemanticError::InvalidCombinationOfModifiers(0, prevInherit + " " + modifierToString(m->type));
            }
            prevInherit = modifierToString(m->type);
        } else if (m->isOverrideModifier()) {
            if (overrided) {
                throw SemanticError::InvalidCombinationOfModifiers(0, modifierToString(m->type) + " " + modifierToString(m->type));
            }
            overrided = true;
        }
    }
}

void ClassDefNode::validatePrimaryConstructorModifiers() const {
    if (!primaryConstructModifier) {
        return;
    }

    if (!primaryConstructModifier->isAccessModifier()) {
        throw SemanticError::InvalidCombinationOfModifiers(0, modifierToString(primaryConstructModifier->type));
    }

    validatePrimaryConstructorParametersModifiers();
}

void ClassDefNode::validatePrimaryConstructorParametersModifiers() const {
    if (!classParams) {
        return;
    }

    string prevAccess;
    string prevInherit;
    bool overrided = false;
    for (ClassParamNode* cp: *classParams->classParams) {
        for (ModifierNode *m: *cp->modifiers->modifiers) {
            if (m->isAccessModifier()) {
                if (!prevAccess.empty()) {
                    throw SemanticError::InvalidCombinationOfModifiers(0, prevAccess + " " + modifierToString(m->type));
                }
                prevAccess = modifierToString(m->type);
            } else if (m->isInheritModifier()) {
                if (m->type == _SEALED) {
                    throw SemanticError::InvalidCombinationOfModifiers(0, modifierToString(m->type));
                }
                if (!prevInherit.empty()) {
                    throw SemanticError::InvalidCombinationOfModifiers(0, prevInherit + " " + modifierToString(m->type));
                }
                prevInherit = modifierToString(m->type);
            } else if (m->isOverrideModifier()) {
                if (cp && cp->type == _VAR_CLASS_PARAM) {
                    throw SemanticError::InvalidCombinationOfModifiers(0, modifierToString(m->type) + " to var");
                }

                if (overrided) {
                    throw SemanticError::InvalidCombinationOfModifiers(0, modifierToString(m->type) + " " + modifierToString(m->type));
                }
                overrided = true;
            }
        }
    }


    if (!primaryConstructModifier->isAccessModifier()) {
        throw SemanticError::InvalidCombinationOfModifiers(0, modifierToString(primaryConstructModifier->type));
    }
}

void TemplateStatNode::validateSecondaryConstructorModifiers() const {
    ModifiersNode* modifiers;
    if (def && def->funDef && def->funDef->constrExpr) {
        modifiers = def->modifiers;
    } else {
        return;
    }

    string prevAccess;
    for (ModifierNode* m: *modifiers->modifiers) {
        if (m->isAccessModifier()) {
            if (!prevAccess.empty()) {
                throw SemanticError::InvalidCombinationOfModifiers(0, prevAccess + " " + modifierToString(m->type));
            }
            prevAccess = modifierToString(m->type);
        } else if (m->isInheritModifier()) {
            throw SemanticError::InvalidCombinationOfModifiers(0, modifierToString(m->type));
        } else if (m->isOverrideModifier()) {
            throw SemanticError::InvalidCombinationOfModifiers(0, modifierToString(m->type));
        }
    }
}

void TopStatNode::secondaryConstructorsToMethods() {
    if (!tmplDef) return;
    if (!tmplDef->classDef) return;

    ClassDefNode *currentClass = tmplDef->classDef;

    if (!currentClass->classTemplateOpt) return;
    if (!currentClass->classTemplateOpt->templateStats) return;

    SimpleExpr1Node* otherConstructorName = SimpleExpr1Node::createIdNode(IdNode::createId("this"));
    // Конструкторы в Scala не имеют явного типа возврата, но для внутреннего представления используем Unit
    SimpleTypeNode* constrReturnType = nullptr;
    for (TemplateStatNode* p: *(currentClass->classTemplateOpt->templateStats->templateStats)) {
        if (!p) continue;
        if (p->dcl) continue;
        if (!p->def->funDef) continue;
        if (!p->def->funDef->constrExpr) continue;
        if (!p->def->funDef->funcParams) continue;

        // Собираем тело конструктора: склеиваем вызов другого конструктора с остальным содержимым тела конструктора
        BlockStatsNode *blockStats = BlockStatsNode::addBlockStatToList(nullptr, nullptr);
        BlockStatNode *otherConstructorCall = BlockStatNode::createSimpleExpr1(SimpleExpr1Node::createMethodCallNode(
                otherConstructorName->copy(), p->def->funDef->constrExpr->argumentExprs->copy())
        );
        BlockStatsNode::addBlockStatToList(blockStats, otherConstructorCall);
        if (p->def->funDef->constrExpr->blockStats) {
            for (BlockStatNode *bs: *(p->def->funDef->constrExpr->blockStats->blockStats)) {
                BlockStatsNode::addBlockStatToList(blockStats, bs->copy());
            }
        }
        ExprNode* bodyOfConstructor = BlockStatNode::createExpr(SimpleExprNode::createBlockStatsNode(blockStats));

        FunSigNode *constrSignature = FunSigNode::createFunSig(IdNode::createId("this"), p->def->funDef->funcParams->copy());
        p->def->funDef->funcParams = nullptr;
        FunDefNode *constructorNode = FunDefNode::createFunSigFunDef(
                constrSignature,
                constrReturnType->copy(),
                bodyOfConstructor
        );
        p->def->funDef->constrExpr = nullptr;
        p->def->funDef = constructorNode;
    }
}

void normalizeInfixes(Node* node) {
    if (!node) return;

    if (auto inf = dynamic_cast<InfixExprNode*>(node)) {
        inf->normalizeInfix();
    }

    auto children = node->getChildren();
    for (Node* child : children) {
        normalizeInfixes(child);
    }
}

static bool isRightAssoc(const std::string &name) {
    return !name.empty() && name.back() == ':';
}

void InfixExprNode::normalizeInfix() {
    if (visited) return;
    if (right) right->normalizeInfix();
    if (left) left->normalizeInfix();
    visited = true;
    if (!left || !right) return;

    if (isRightAssoc(fullId->name)) {
        InfixExprNode* tmp = left;
        left = right;
        right = tmp;
    }
}

void transformInfixes(Node* node) {
    if (!node) return;

    if (auto inf = dynamic_cast<InfixExprNode*>(node)) {
        if (!inf->visitedForInfixOpTransform) {
            inf->transformInfixOperationToMethodCall();
        }
    }

    auto children = node->getChildren();
    for (Node* child : children) {
        transformInfixes(child);
    }
}

static int iter = 1;

void InfixExprNode::transformInfixOperationToMethodCall() {
    if (visitedForInfixOpTransform) return;
    visitedForInfixOpTransform = true;
    if (!left || !right) return;

    right->transformInfixOperationToMethodCall();
    left->transformInfixOperationToMethodCall();

    std::cout << "start " + to_string((iter++)) + "\n";

    prefixExpr = new PrefixExprNode();
    prefixExpr->type = left->prefixExpr ? left->prefixExpr->type : _NO_UNARY_OPERATOR;

    prefixExpr->simpleExpr = new SimpleExprNode();
    prefixExpr->simpleExpr->type = _SIMPLE_EXPR_1;

    prefixExpr->simpleExpr->simpleExpr1 = new SimpleExpr1Node();
    prefixExpr->simpleExpr->simpleExpr1->type = _METHOD_CALL;
    prefixExpr->simpleExpr->simpleExpr1->simpleExpr1 = new SimpleExpr1Node();
    prefixExpr->simpleExpr->simpleExpr1->simpleExpr1->simpleExpr = left->prefixExpr->simpleExpr;
    prefixExpr->simpleExpr->simpleExpr1->simpleExpr1 = SimpleExpr1Node::createIdNode(fullId->copy());
    prefixExpr->simpleExpr->simpleExpr1->simpleExpr1->type = _EXPRESSION_FIELD_ACCESS;
    prefixExpr->simpleExpr->simpleExpr1->simpleExpr1->simpleExpr = new SimpleExprNode();
    prefixExpr->simpleExpr->simpleExpr1->simpleExpr1->simpleExpr->type = _SIMPLE_EXPR_1;
    prefixExpr->simpleExpr->simpleExpr1->simpleExpr1->simpleExpr->simpleExpr1 = new SimpleExpr1Node();
    prefixExpr->simpleExpr->simpleExpr1->simpleExpr1->simpleExpr->simpleExpr1 = left->prefixExpr->simpleExpr->simpleExpr1;

    ExprNode* argument = ExprNode::createInfix(right);
    ExprsNode* argumentAsList = new ExprsNode(argument);
    prefixExpr->simpleExpr->simpleExpr1->argumentExprs = new ArgumentExprsNode(
            argumentAsList
    );

    left = nullptr;
    right = nullptr;
    fullId = nullptr;

    std::cout << "end\n";
}
