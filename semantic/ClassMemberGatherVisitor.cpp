#include "ClassMemberGatherVisitor.h"
#include <iostream>

#include "tables/tables.hpp"

ClassMemberGatherVisitor::ClassMemberGatherVisitor() : currentClass(nullptr) {}

void ClassMemberGatherVisitor::visitTree(TopStatSeqNode* root) {
    if (root) visit(root);
}

void ClassMemberGatherVisitor::visit(Node* node) {
    if (!node) return;

    if (auto* classDef = dynamic_cast<ClassDefNode*>(node)) {
        visitClassDef(classDef);
        return;
    }

    if (auto* topStat = dynamic_cast<TopStatNode*>(node)) {
         visitChildren(node);
         return;
    }

    // Если мы внутри класса
    if (currentClass) {
        if (auto* tmplStat = dynamic_cast<TemplateStatNode*>(node)) {
            // TemplateStat -> DefNode / DclNode.
            // Они сами содержат модификаторы.
            visitTemplateStat(tmplStat);
            return;
        }

        if (auto* defNode = dynamic_cast<DefNode*>(node)) {
            visitDef(defNode);
            return;
        }
        if (auto* dclNode = dynamic_cast<DclNode*>(node)) {
            visitDcl(dclNode);
            return;
        }
    }

    visitChildren(node);
}

void ClassMemberGatherVisitor::visitChildren(Node* node) {
    for (Node* child : node->getChildren()) {
        visit(child);
    }
}


void ClassMemberGatherVisitor::visitClassDef(ClassDefNode* node) {
    if (!node->fullId) return;

    auto result = ctx().addClass(node);
    if (!result.has_value()) {
        // TODO
        std::cerr << "Error: Class " << node->fullId->name << " already defined." << std::endl;
        return;
    }

    ClassMetaInfo* newClassInfo = result.value();

    ClassMetaInfo* prevClass = currentClass;
    currentClass = newClassInfo;

    // 2. Тело класса
    if (node->classTemplateOpt) {
        visit(node->classTemplateOpt);
    }

    currentClass = prevClass;
}


void ClassMemberGatherVisitor::visitTemplateStat(TemplateStatNode* node) {
    visitChildren(node);
}

void ClassMemberGatherVisitor::visitDef(DefNode* node) {
    if (!currentClass) return;
    auto mod = Modifiers::createFromModifiersNode(*node->modifiers);
    if (node->type == _VAR_DEFS && node->varDefs) {
        currentClass->addField(node->varDefs, mod);
    }
    else if (node->type == _FUN_DEFINITION && node->funDef) {
        currentClass->addMethod(node->funDef, mod);
    }
}

void ClassMemberGatherVisitor::visitDcl(DclNode* node) {
    if (!currentClass) return;
    auto mod = Modifiers::createFromModifiersNode(*node->modifiers);
    if (node->type == _VAL_DECL || node->type == _VAR_DECL) {
        currentClass->addField(node);
    }
    else if (node->type == _FUN_DEF) {
        currentClass->addMethod(node);
    }
}