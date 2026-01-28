#include "ClassMemberGatherVisitor.h"
#include <iostream>

#include "tables/tables.hpp"
#include "error/ErrorTable.h"
#include "tools/datatype.h"

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
        ErrorTable::addErrorToList(new SemanticError(SemanticError::ClassRedefinition(0, node->fullId->name)));
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
        auto result = currentClass->addField(node->varDefs, mod);
        if (!result.has_value() && node->varDefs->fullId) {
            int line = node->varDefs->fullId->id;
            std::string fieldName = node->varDefs->fullId->name;
            ErrorTable::addErrorToList(new SemanticError(
                SemanticError::FieldRedefinition(line, fieldName)
            ));
        }
    }
    else if (node->type == _FUN_DEFINITION && node->funDef) {
        auto result = currentClass->addMethod(node->funDef, mod);
        if (!result.has_value() && node->funDef->funSig && node->funDef->funSig->fullId) {
            int line = node->funDef->funSig->fullId->id;
            std::string methodName = node->funDef->funSig->fullId->name;
            // Build signature string with argument types
            std::string sigStr = methodName + "(";
            if (node->funDef->funSig->params && node->funDef->funSig->params->funcParams) {
                bool first = true;
                for (auto* paramNode : *node->funDef->funSig->params->funcParams) {
                    if (!first) sigStr += ", ";
                    first = false;
                    sigStr += DataType::createFromNode(paramNode->simpleType).toString();
                }
            }
            sigStr += ")";
            ErrorTable::addErrorToList(new SemanticError(
                SemanticError::MethodAlreadyExists(line, sigStr)
            ));
        }
    }
}

void ClassMemberGatherVisitor::visitDcl(DclNode* node) {
    if (!currentClass) return;
    auto mod = Modifiers::createFromModifiersNode(*node->modifiers);
    if (node->type == _VAL_DECL || node->type == _VAR_DECL) {
        auto result = currentClass->addField(node);
        if (!result.has_value() && node->fullId) {
            int line = node->fullId->id;
            std::string fieldName = node->fullId->name;
            ErrorTable::addErrorToList(new SemanticError(
                SemanticError::FieldRedefinition(line, fieldName)
            ));
        }
    }
    else if (node->type == _FUN_DEF) {
        auto result = currentClass->addMethod(node);
        if (!result.has_value() && node->funSig && node->funSig->fullId) {
            int line = node->funSig->fullId->id;
            std::string methodName = node->funSig->fullId->name;
            // Build signature string with argument types
            std::string sigStr = methodName + "(";
            if (node->funSig->params && node->funSig->params->funcParams) {
                bool first = true;
                for (auto* paramNode : *node->funSig->params->funcParams) {
                    if (!first) sigStr += ", ";
                    first = false;
                    sigStr += DataType::createFromNode(paramNode->simpleType).toString();
                }
            }
            sigStr += ")";
            ErrorTable::addErrorToList(new SemanticError(
                SemanticError::MethodAlreadyExists(line, sigStr)
            ));
        }
    }
}