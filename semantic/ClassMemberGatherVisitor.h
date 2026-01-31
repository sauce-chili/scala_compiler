//
// Created by Dmitry Smirnov on 15.01.2026.
//

#ifndef SCALA_LEXER_CLASSMEMBERGATHERVISITOR_H
#define SCALA_LEXER_CLASSMEMBERGATHERVISITOR_H

#include "nodes/nodes.h"
#include "semantic/SemanticContext.h"

class Modifiers;

class ClassMemberGatherVisitor {
private:
    // Текущий обрабатываемый класс
    ClassMetaInfo* currentClass = nullptr;

    void visit(Node* node);
    void visitChildren(Node* node);

    void visitClassDef(ClassDefNode* node);
    void visitTemplateStat(TemplateStatNode* node);

    void validateArgumentNames(FuncParamsNode* funcParams);

    // Узлы членов
    void visitDef(DefNode* node);
    void visitDcl(DclNode* node);

public:
    ClassMemberGatherVisitor();
    void visitTree(TopStatSeqNode* root);
};

#endif //SCALA_LEXER_CLASSMEMBERGATHERVISITOR_H