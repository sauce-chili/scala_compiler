#ifndef SCALA_LEXER_TEMPLATESTATNODE_H
#define SCALA_LEXER_TEMPLATESTATNODE_H

#include "../Node.h"
class DefNode;
class ModifiersNode;
class DclNode;

class TemplateStatNode: public Node{
public:
    DefNode* def;
    DclNode* dcl;

    TemplateStatNode();

    static TemplateStatNode* createDefTemplate(ModifiersNode* modifiers, DefNode* def);
    static TemplateStatNode* createDclTemplate(ModifiersNode* modifiers, DclNode* dcl);

    TemplateStatNode* copy();
    void validateModifiers() const;

    string toDot() const override;
    string getDotLabel() const override;
    list<Node *> getChildren() const override;

private:
    void validateVarModifiers() const;
    void validateMethodModifiers() const;
    void validateSecondaryConstructorModifiers() const;
    void validatePrimaryConstructorModifiers() const;
};


#endif //SCALA_LEXER_TEMPLATESTATNODE_H
