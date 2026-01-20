#ifndef SCALA_LEXER_TOPSTATNODE_H
#define SCALA_LEXER_TOPSTATNODE_H


#include "../Node.h"
#include "../modifier/ModifiersNode.h"
#include "../templates/TemplateDefNode.h"

class TopStatNode: public Node {
public:
    ModifiersNode* modifiers;
    TemplateDefNode* templateDef;

    TopStatNode();

    static TopStatNode* createClass(ModifiersNode* modifiers, TemplateDefNode* templateDef);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_TOPSTATNODE_H
