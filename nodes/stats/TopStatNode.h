#ifndef SCALA_LEXER_TOPSTATNODE_H
#define SCALA_LEXER_TOPSTATNODE_H


#include "../Node.h"
#include "../modifier/ModifiersNode.h"
#include "../templates/TemplateDefNode.h"
#include "../class/ClassParamsNode.h"
#include "common/CommonConstants.h"

class TopStatNode: public Node {
public:
    TemplateDefNode* tmplDef;

    TopStatNode();

    static TopStatNode* createClass(ModifiersNode* modifiers, TemplateDefNode* templateDef);

    void toFieldsFromPrimaryConstructor();
    void initializeBaseConstructor();
    void secondaryConstructorsToMethods();
    list<BlockStatNode*> initializeVarFromClassParams(ClassParamsNode* classParams);

    TopStatNode* copy();

    string toDot() const override;
    string getDotLabel() const override;
    list<Node *> getChildren() const override;
};


#endif //SCALA_LEXER_TOPSTATNODE_H
