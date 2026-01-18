#ifndef SCALA_LEXER_CLASSTEMPLATEOPTNODE_H
#define SCALA_LEXER_CLASSTEMPLATEOPTNODE_H


#include "../Node.h"
#include "../class/ClassParentsNode.h"
#include "../templates/TemplateStatsNode.h"

class ClassTemplateOptNode: public Node {
public:
    TemplateStatsNode* templateStats;
    ClassParentsNode* classParents;

    ClassTemplateOptNode();

    static ClassTemplateOptNode* addFuncParamToBackToList(TemplateStatsNode* templateStats);
    static ClassTemplateOptNode* createClassTemplate(ClassParentsNode* classParents, TemplateStatsNode* templateStats);

    ClassTemplateOptNode* copy();

    string toDot() const override;
    string getDotLabel() const override;
    list<Node *> getChildren() const override;
};


#endif //SCALA_LEXER_CLASSTEMPLATEOPTNODE_H
