#ifndef SCALA_LEXER_TOPSTATSEQNODE_H
#define SCALA_LEXER_TOPSTATSEQNODE_H


#include "../Node.h"
#include "TopStatNode.h"

class TopStatSeqNode: public Node {
public:
    std::list<TopStatNode*> *topStats;

    TopStatSeqNode();
    TopStatSeqNode(TopStatNode* topStat);

    static TopStatSeqNode* addModifierToList(TopStatSeqNode *list, TopStatNode *topStat);

    void convertAst();

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_TOPSTATSEQNODE_H
