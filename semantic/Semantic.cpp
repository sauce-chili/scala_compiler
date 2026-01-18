#include "nodes/stats/TopStatSeqNode.h"
#include "nodes/class/ClassDefNode.h"
#include "nodes/definitions/DclNode.h"
#include "nodes/definitions/DefNode.h"
#include "nodes/exprs/ArgumentExprsNode.h"
#include "nodes/stats/TopStatNode.h"
#include "semantic/error/SemanticError.h"

#include <stdexcept>

void TopStatSeqNode::convertAst() {
    for (TopStatNode* tsn: *(topStats)) {
        if (!tsn) continue;
        if (!tsn->tmplDef) continue;
        if (!tsn->tmplDef->classDef) continue;

        tsn->initializeClassModifiers();
        tsn->toFieldsFromPrimaryConstructor();
        tsn->initializeBaseConstructorFromFields();
    }
}

void TopStatNode::initializeClassModifiers() {
//    if (!modifiers || modifiers->modifiers->empty()) {
//        modifiers = modifiers->addModifierToList(modifiers, ModifierNode::createModifier(ModifierType::_PUBLIC));
//    }

    ClassDefNode *currentClass = tmplDef->classDef;
    currentClass->primaryConstructModifier = currentClass->primaryConstructModifier ? currentClass->primaryConstructModifier : ModifierNode::createModifier(ModifierType::_PUBLIC);
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
        IdsNode *ids = IdsNode::addIdToList(nullptr, p->fullId->copy());
        if (p->type == ClassParamType::_VAR_CLASS_PARAM) {
            stat->dcl = DclNode::createVarDcl(ids, p->compoundType->copy());
        } else {
            stat->dcl = DclNode::createValDcl(ids, p->compoundType->copy());
        }
        stat->dcl->modifiers = stat->dcl->modifiers->addModifierToList(stat->dcl->modifiers, ModifierNode::createModifier(_PUBLIC));

        currentClass->classTemplateOpt->templateStats->templateStats->push_front(stat);
    }

    currentClass->classParams = new ClassParamsNode();
}

void TopStatNode::initializeBaseConstructorFromFields() {
    if (!tmplDef) return;
    if (!tmplDef->classDef) return;

    ClassDefNode *currentClass = tmplDef->classDef;

    if (!currentClass->classTemplateOpt) return;
    if (!currentClass->classTemplateOpt->templateStats) return;

    if (!currentClass->classParams) currentClass->classParams = new ClassParamsNode();

    // Сущность конструктора
    TemplateStatNode *baseConstructor = new TemplateStatNode();
    // Скобки с параметрами
    FuncParamsNode *params = new FuncParamsNode(currentClass->classParams);
    // Вызов родительского конструктора (будет всегда, как минимум Object)
    SuperConstructorCallNode *superCall;
    if (currentClass->classTemplateOpt->classParents && currentClass->classTemplateOpt->classParents->constr && currentClass->classTemplateOpt->classParents->constr->arguments) {
        superCall = new SuperConstructorCallNode(currentClass->classTemplateOpt->classParents->constr->arguments->copy());
    } else {
        superCall = new SuperConstructorCallNode(new ArgumentExprsNode(new ExprsNode()));
    }
    // Поля класса
    BlockStatsNode *blockStats = BlockStatsNode::addBlockStatToList(nullptr, nullptr);
    for (TemplateStatNode* p: *(currentClass->classTemplateOpt->templateStats->templateStats)) {
        if (!p) continue;
        if (p->dcl) continue;

        BlockStatNode *stat;
        if (p->def->varDefs->type == StatType::_VAR_DECL) {
            stat = BlockStatNode::createVarDefsNode(
                    VarDefsNode::createVar(p->def->varDefs->ids, p->def->varDefs->compoundType, p->def->varDefs->expr));
        } else {
            stat = BlockStatNode::createVarDefsNode(
                    VarDefsNode::createVal(p->def->varDefs->ids, p->def->varDefs->compoundType, p->def->varDefs->expr));
        }
        BlockStatsNode::addBlockStatToList(blockStats, stat);
    }

    PrimaryConstructorNode *primaryConstructorNode = new PrimaryConstructorNode(params, blockStats, superCall);
    baseConstructor->def = DefNode::createPrimaryConstructor(primaryConstructorNode);
    currentClass->classTemplateOpt->templateStats->templateStats->push_front(baseConstructor);

    currentClass->classTemplateOpt->classParents->constr->arguments = nullptr;
}