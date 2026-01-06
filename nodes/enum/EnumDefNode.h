#ifndef SCALA_LEXER_ENUMDEFNODE_H
#define SCALA_LEXER_ENUMDEFNODE_H


#include "../Node.h"
#include "../id/IdNode.h"
#include "../class/ClassParamsNode.h"
#include "../templates/EnumTemplateNode.h"

//fullID accessModifier classParamClause enumTemplate
//       | fullID classParamClause enumTemplate
//       | fullID enumTemplate
class EnumDefNode: public Node {
public:
    IdNode* id;
    ModifierNode* modifier;
    ClassParamsNode* classParams;
    EnumTemplateNode* enumTemplate;

    EnumDefNode();

    static EnumDefNode* createWithAccessModifier(
            IdNode* id,
            ModifierNode* modifier,
            ClassParamsNode* classParams,
            EnumTemplateNode* enumTemplate
            );

    static EnumDefNode* createWithClassParams(
            IdNode* id,
            ClassParamsNode* classParams,
            EnumTemplateNode* enumTemplate
    );

    static EnumDefNode* createEnumTemplate(
            IdNode* id,
            EnumTemplateNode* enumTemplate
    );

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_ENUMDEFNODE_H
