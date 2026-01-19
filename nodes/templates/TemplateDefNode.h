#ifndef SCALA_LEXER_TEMPLATEDEFNODE_H
#define SCALA_LEXER_TEMPLATEDEFNODE_H

#include "../Node.h"
#include "nodes/modifier/ModifiersNode.h"
#include "../semantic/scopes/Scope.h"

class Scope;
class ClassDefNode;
class ClassTemplateOptNode;
class IdNode;
class TraitTemplateOptNode;
class EnumDefNode;


class TemplateDefNode: public Node {
public:
    ModifiersNode* modifiers;
    StatType type;
    ClassDefNode* classDef;
    IdNode* fullId;
    ClassTemplateOptNode* classTemplateOpt;
    TraitTemplateOptNode* traitTemplateOpt;
    EnumDefNode* enumDef;
    Scope* classScope; // тип класс, сразу назначать имя

    TemplateDefNode();

    static TemplateDefNode* createClassDef(ClassDefNode* classDef);
    static TemplateDefNode* createObjectDef(IdNode* fullId, ClassTemplateOptNode* classTemplateOpt);
    static TemplateDefNode* createTraitDef(IdNode* fullId, TraitTemplateOptNode* traitTemplateOpt);
    static TemplateDefNode* createEnumDef(EnumDefNode* enumDef);

    TemplateDefNode* copy();

    string toDot() const override;
    string getDotLabel() const override;

    TemplateDefNode* setModifiers(ModifiersNode* modifiers);
    void validateModifiers() const;

    list<Node *> getChildren() const override;

private:
    void validateClassModifiers() const;
};


#endif //SCALA_LEXER_TEMPLATEDEFNODE_H
