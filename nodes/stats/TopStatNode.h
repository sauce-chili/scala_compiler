#ifndef SCALA_LEXER_TOPSTATNODE_H
#define SCALA_LEXER_TOPSTATNODE_H


#include "../Node.h"
#include "../modifier/ModifiersNode.h"
#include "../templates/TemplateDefNode.h"

extern const std::string CONSTRUCTOR_NAME;
extern const std::string JVM_CONSTRUCTOR_NAME;

class TopStatNode: public Node {
public:
    TemplateDefNode* tmplDef;

    TopStatNode();

    static TopStatNode* createClass(ModifiersNode* modifiers, TemplateDefNode* templateDef);

    void toFieldsFromPrimaryConstructor();
    void initializeBaseConstructorFromFields() const;
    void secondaryConstructorsToMethods();

    TopStatNode* copy();

    string toDot() const override;
    string getDotLabel() const override;
    list<Node *> getChildren() const override;
};


#endif //SCALA_LEXER_TOPSTATNODE_H
