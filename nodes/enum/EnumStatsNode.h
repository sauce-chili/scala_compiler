#ifndef SCALA_LEXER_ENUMSTATSNODE_H
#define SCALA_LEXER_ENUMSTATSNODE_H

#include <list>
#include "../Node.h"
#include "EnumStatNode.h"

class EnumStatsNode: public Node {
public:
    std::list<EnumStatNode*> *enumStats;

    EnumStatsNode();
    EnumStatsNode(EnumStatNode* enumStat);

    static EnumStatsNode* addModifierToList(EnumStatsNode *list, EnumStatNode *enumStat);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_ENUMSTATSNODE_H
