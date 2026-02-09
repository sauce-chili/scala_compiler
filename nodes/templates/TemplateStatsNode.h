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

    static TemplateStatsNode* addTemplateStatToBackToList(TemplateStatsNode *list, TemplateStatNode *templateStat);
    static TemplateStatsNode* addTemplateStatToFrontToList(TemplateStatsNode *list, TemplateStatNode *templateStat);

    TemplateStatsNode* copy();
    void validateModifiers() const;

    bool containsVar(string name);

    string toDot() const override;
    string getDotLabel() const override;
    list<Node *> getChildren() const override;
};


#endif //SCALA_LEXER_TEMPLATESTATSNODE_H
