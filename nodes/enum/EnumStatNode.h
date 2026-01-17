#ifndef SCALA_LEXER_ENUMSTATNODE_H
#define SCALA_LEXER_ENUMSTATNODE_H


#include "../Node.h"
#include "../templates/TemplateStatNode.h"
#include "../enum/EnumCaseNode.h"

class EnumStatNode: public Node {
public:
    TemplateStatNode* templateStat;
    ModifiersNode* modifiers;
    EnumCaseNode* enumCase;

    EnumStatNode();

    static EnumStatNode* createWithTemplateStat(TemplateStatNode* templateStat);
    static EnumStatNode* createWithEnumCase(ModifiersNode* modifiers, EnumCaseNode* enumCase);

    string toDot() const override;
    string getDotLabel() const override;
    list<Node *> getChildren() const override;
};


#endif //SCALA_LEXER_ENUMSTATNODE_H
