//
// Created by Roman on 1/5/2026.
//

#ifndef SCALA_LEXER_TEMPLATESTATNODE_H
#define SCALA_LEXER_TEMPLATESTATNODE_H

#include "../Node.h"
#include "../modifier/ModifiersNode.h"
#include "../definitions/DefNode.h"
#include "../definitions/DclNode.h"

class TemplateStatNode: public Node{
public:
    ModifiersNode* modifiers;
    DefNode* def;
    DclNode* dcl;

    TemplateStatNode();

    static TemplateStatNode* createDefTemplate(ModifiersNode* modifiers, DefNode* def);
    static TemplateStatNode* createDclTemplate(ModifiersNode* modifiers, DclNode* dcl);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_TEMPLATESTATNODE_H
