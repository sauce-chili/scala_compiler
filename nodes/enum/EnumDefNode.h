#ifndef SCALA_LEXER_ENUMDEFNODE_H
#define SCALA_LEXER_ENUMDEFNODE_H


#include "../Node.h"
#include "../id/IdNode.h"
#include "../class/ClassParamsNode.h"
#include "../templates/EnumTemplateNode.h"

class EnumDefNode : public Node {
public:
    IdNode *fullId;
    ModifierNode *modifier;
    ClassParamsNode *classParams;
    EnumTemplateNode *enumTemplate;

    EnumDefNode();

    static EnumDefNode *createWithAccessModifier(
        IdNode *fullId,
        ModifierNode *modifier,
        ClassParamsNode *classParams,
        EnumTemplateNode *enumTemplate
    );

    static EnumDefNode *createWithClassParams(
        IdNode *fullId,
        ClassParamsNode *classParams,
        EnumTemplateNode *enumTemplate
    );

    static EnumDefNode *createEnumTemplate(
        IdNode *fullId,
        EnumTemplateNode *enumTemplate
    );

    string toDot() const override;

    string getDotLabel() const override;

    list<Node *> getChildren() const override;
};


#endif //SCALA_LEXER_ENUMDEFNODE_H
