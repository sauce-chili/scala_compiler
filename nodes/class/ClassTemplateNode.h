#ifndef SCALA_LEXER_CLASSTEMPLATENODE_H
#define SCALA_LEXER_CLASSTEMPLATENODE_H

#include "../Node.h"
#include "ClassParentsNode.h"
#include "../templates/TemplateStatsNode.h"

class ClassTemplateNode: public Node {
public:
    ClassParentsNode* classParents;
    TemplateStatsNode* templateStats;

    ClassTemplateNode();

    static ClassTemplateNode* createClassTemplate(ClassParentsNode* classParents, TemplateStatsNode* templateStats);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_CLASSTEMPLATENODE_H
