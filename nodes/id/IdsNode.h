#ifndef SCALA_LEXER_IDSNODE_H
#define SCALA_LEXER_IDSNODE_H


#include "../Node.h"
#include "IdNode.h"
#include <list>

class IdsNode: public Node {
public:
    std::list<IdNode*> *ids;

    IdsNode();
    IdsNode(IdNode* id);

    static IdsNode* addIdToList(IdsNode *list, IdNode *id);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_IDSNODE_H
