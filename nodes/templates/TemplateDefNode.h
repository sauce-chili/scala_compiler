#ifndef SCALA_LEXER_TEMPLATEDEFNODE_H
#define SCALA_LEXER_TEMPLATEDEFNODE_H

#include "../Node.h"
#include "nodes/modifier/ModifiersNode.h"
#include "../semantic/scopes/Scope.h"

class ClassDefNode;
class ClassTemplateOptNode;
class IdNode;


class TemplateDefNode: public Node {
public:
    StatType type;
    ClassDefNode* classDef;

    TemplateDefNode();

    static TemplateDefNode* createClassDef(ClassDefNode* classDef);

    TemplateDefNode* copy();

    string toDot() const override;
    string getDotLabel() const override;

    TemplateDefNode* setModifiers(ModifiersNode* modifiers);
    ModifiersNode * getModifiers() const;
    void validateModifiers() const;

    list<Node *> getChildren() const override;

private:
    void validateClassModifiers() const;
};


#endif //SCALA_LEXER_TEMPLATEDEFNODE_H
