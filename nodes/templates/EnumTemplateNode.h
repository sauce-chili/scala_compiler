#ifndef SCALA_LEXER_ENUMTEMPLATENODE_H
#define SCALA_LEXER_ENUMTEMPLATENODE_H


#include "../Node.h"
#include "../class/ClassParentsNode.h"
#include "../enum/EnumStatsNode.h"

class EnumTemplateNode: public Node {
public:
    ClassParentsNode* classParents;
    EnumStatsNode* enumStats;

    EnumTemplateNode();

    static EnumTemplateNode* createWithClassParents(ClassParentsNode* classParents, EnumStatsNode* enumStats);
    static EnumTemplateNode* createWithEnumBody(EnumStatsNode* enumStats);

    EnumTemplateNode* copy();

    string toDot() const override;
    string getDotLabel() const override;
    list<Node *> getChildren() const override;
};


#endif //SCALA_LEXER_ENUMTEMPLATENODE_H
