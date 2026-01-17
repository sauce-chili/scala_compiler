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

    string toDot() const override;
    string getDotLabel() const override;
    list<Node *> getChildren() const override;
};


#endif //SCALA_LEXER_TEMPLATESTATNODE_H
