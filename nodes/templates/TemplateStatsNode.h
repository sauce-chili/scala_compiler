#ifndef SCALA_LEXER_TEMPLATESTATSNODE_H
#define SCALA_LEXER_TEMPLATESTATSNODE_H

#include <list>

#include "../Node.h"
#include "TemplateStatNode.h"

class TemplateStatsNode: public Node {
public:
    std::list<TemplateStatNode*> *templateStats;

    TemplateStatsNode();
    TemplateStatsNode(TemplateStatNode* first);

    static TemplateStatsNode* addFuncParamToBackToList(TemplateStatsNode *list, TemplateStatNode *templateStat);
    static TemplateStatsNode* addFuncParamToFrontToList(TemplateStatsNode *list, TemplateStatNode *templateStat);

    string toDot() const override;
    string getDotLabel() const override;
    list<Node *> getChildren() const override;
};


#endif //SCALA_LEXER_TEMPLATESTATSNODE_H
