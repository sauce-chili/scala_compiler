//
// Created by Roman on 1/5/2026.
//

#ifndef SCALA_LEXER_SINGLESTABLEIDNODE_H
#define SCALA_LEXER_SINGLESTABLEIDNODE_H

#include "../Node.h"
#include "IdNode.h"
#include "../Types.h"

class SingleStableIdNode: public Node {
public:
    StableIdType type;
    IdNode* identifier;

    static SingleStableIdNode* createStableIdByFullId(IdNode* identifier);
    static SingleStableIdNode* createSuperCallStableId(IdNode* identifier);
    static SingleStableIdNode* createThisCallStableIdBy(IdNode* identifier);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_SINGLESTABLEIDNODE_H
