#include "nodes/stats/TopStatSeqNode.h"
#include "nodes/class/ClassDefNode.h"
#include "nodes/definitions/DclNode.h"
#include "nodes/definitions/DefNode.h"
#include "nodes/exprs/ArgumentExprsNode.h"
#include "nodes/stats/TopStatNode.h"
#include "nodes/class/ClassParamNode.h"
#include "nodes/class/ClassParamsNode.h"
#include "semantic/error/SemanticError.h"

#include <stdexcept>

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
    }
}

void ClassDefNode::normalizeBody() const {
    if (classTemplateOpt && classTemplateOpt->extensionPartClassTemplate && classTemplateOpt->extensionPartClassTemplate->templateStats) {
        classTemplateOpt->templateStats = classTemplateOpt->extensionPartClassTemplate->templateStats;
        classTemplateOpt->extensionPartClassTemplate->templateStats = nullptr;
    }
}


//void ModifiersNode::initializeModifiers(Node* node) {
//    if (!node) return;
//
//    if (auto* mods = dynamic_cast<ModifiersNode*>(node)) {
//        if (!mods->modifiers || mods->modifiers->empty()) {
//            if (!mods->modifiers) {
//                mods->modifiers = new std::list<ModifierNode *>();
//            }
//
////            mods->modifiers->push_back(
////                    ModifierNode::createModifier(_PUBLIC)
////            );
//        }
//    }
//
//    for (Node* child : node->getChildren()) {
//        initializeModifiers(child);
//    }
//}

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
        IdsNode *ids = IdsNode::addIdToList(nullptr, p->fullId->copy());
        if (p->type == ClassParamType::_VAR_CLASS_PARAM) {
            stat->dcl = DclNode::createVarDcl(ids, p->simpleType->copy());
        } else {
            stat->dcl = DclNode::createValDcl(ids, p->simpleType->copy());
        }
        stat->dcl->modifiers = p->modifiers->copy();

        currentClass->classTemplateOpt->templateStats->templateStats->push_front(stat);
    }
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
    // Скобки с параметрами
    FuncParamsNode *params = new FuncParamsNode(currentClass->classParams);
    // Вызов родительского конструктора (будет всегда, как минимум Object)
    SuperConstructorCallNode *superCall;
    if (currentClass->classTemplateOpt->extensionPartClassTemplate && currentClass->classTemplateOpt->extensionPartClassTemplate->argumentExprs) {
        superCall = new SuperConstructorCallNode(currentClass->classTemplateOpt->extensionPartClassTemplate->argumentExprs->copy());
        // Убираем скобки с аргументами конструктора родителя
        currentClass->classTemplateOpt->extensionPartClassTemplate->fullId = currentClass->classTemplateOpt->extensionPartClassTemplate->fullId;
        currentClass->classTemplateOpt->extensionPartClassTemplate->argumentExprs = nullptr;
    } else {
        superCall = new SuperConstructorCallNode(new ArgumentExprsNode(new ExprsNode()));
    }
    // Поля класса
    BlockStatsNode *blockStats = BlockStatsNode::addBlockStatToList(nullptr, nullptr);
    for (TemplateStatNode* p: *(currentClass->classTemplateOpt->templateStats->templateStats)) {
        if (!p) continue;
        if (p->dcl) continue;
        if (!p->def->varDefs) continue;

        BlockStatNode *stat;
        SimpleTypeNode* typeOfVars = p->def->varDefs->simpleType ? p->def->varDefs->simpleType->copy() : nullptr;
        if (p->def->varDefs->type == StatType::_VAR_DECL) {
            stat = BlockStatNode::createVarDefsNode(
                    VarDefsNode::createVar(p->def->varDefs->ids->copy(), typeOfVars, p->def->varDefs->expr->copy()));
        } else {
            stat = BlockStatNode::createVarDefsNode(
                    VarDefsNode::createVal(p->def->varDefs->ids->copy(), typeOfVars, p->def->varDefs->expr->copy()));
        }
        p->def->varDefs->expr = nullptr;
        BlockStatsNode::addBlockStatToList(blockStats, stat);
    }

    PrimaryConstructorNode *primaryConstructorNode = new PrimaryConstructorNode(params, blockStats, superCall);
    currentClass->classParams = new ClassParamsNode();
    baseConstructor->def = DefNode::createPrimaryConstructor(primaryConstructorNode);
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
