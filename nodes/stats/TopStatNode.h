#ifndef SCALA_LEXER_TOPSTATNODE_H
#define SCALA_LEXER_TOPSTATNODE_H


#include "../Node.h"
#include "../modifier/ModifiersNode.h"
#include "../templates/TemplateDefNode.h"

class TopStatNode: public Node {
public:
    TemplateDefNode* tmplDef;

    TopStatNode();

    static TopStatNode* createTopStat(ModifiersNode* modifiers, TemplateDefNode* tmplDef);

    string toDot() const override;
    string getDotLabel() const override;
    list<Node *> getChildren() const override;
};


#endif //SCALA_LEXER_TOPSTATNODE_H
