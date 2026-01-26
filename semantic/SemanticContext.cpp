//
// Created by Dmitry Smirnov on 18.01.2026.
//

#include "SemanticContext.h"

#include "error/ErrorTable.h"
#include "error/SemanticError.h"
#include "nodes/class/ClassDefNode.h"
#include "tables/tables.hpp"

void SemanticContext::ensureRtlInitialized() {
    if (!rtlInitialized) {
        rtlInitialized = true;
        RtlClassMetaInfo::initializeRtlClasses();
    }
}

optional<ClassMetaInfo *> SemanticContext::addRTL(RtlClassMetaInfo *rtlClass) {
    if (!rtlClass) return nullopt;

    string clsName = rtlClass->name;

    // RTL классы не должны вызывать ошибку редефиниции - они первичны
    if (classes.find(clsName) != classes.end()) {
        throw std::logic_error("Переопредление RTL класса");
    }

    classes[clsName] = rtlClass;
    return rtlClass;
}

optional<ClassMetaInfo *> SemanticContext::addClass(ClassDefNode *classDefNode) {
    if (!classDefNode || !classDefNode->fullId) return nullopt;

    string clsName = classDefNode->fullId->name;

    if (classes.find(clsName) != classes.end()) {
        ErrorTable::addErrorToList(new SemanticError(SemanticError::ClassRedefinition(0, clsName)));
        return nullopt;
    }

    Modifiers mods = Modifiers::createFromModifiersNode(*classDefNode->modifiers);
    ClassMetaInfo *clsMeta = new ClassMetaInfo(clsName, mods, classDefNode);

    classes[clsName] = clsMeta;

    return clsMeta;
}

optional<ClassMetaInfo *> SemanticContext::addClass(ClassMetaInfo *classMetaInfo) {
    if (!classMetaInfo) return nullopt;

    string clsName = classMetaInfo->name;

    if (classes.find(clsName) != classes.end()) {
        ErrorTable::addErrorToList(new SemanticError(SemanticError::ClassRedefinition(0, clsName)));
        return nullopt;
    }

    Modifiers mods = classMetaInfo->modifiers.modifiers.empty() ? Modifiers({_PUBLIC}) : classMetaInfo-> modifiers;
    classMetaInfo->modifiers = mods;

    classes[clsName] = classMetaInfo;

    return classMetaInfo;
}
