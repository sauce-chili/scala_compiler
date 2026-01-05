#ifndef SCALA_LEXER_FUNSIGNODE_H
#define SCALA_LEXER_FUNSIGNODE_H


#include "../Node.h"
#include "../id/IdNode.h"
#include "FuncParamsNode.h"

class FunSigNode: public Node {
public:
    IdNode* id;
    FuncParamsNode* params;

    FunSigNode();

    static FunSigNode* createFunSig(IdNode* id, FuncParamsNode* params);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_FUNSIGNODE_H
