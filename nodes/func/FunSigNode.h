#ifndef SCALA_LEXER_FUNSIGNODE_H
#define SCALA_LEXER_FUNSIGNODE_H


#include "../Node.h"
#include "../id/IdNode.h"
#include "FuncParamsNode.h"

class FunSigNode: public Node {
public:
    IdNode* fullId;
    FuncParamsNode* params;

    FunSigNode();

    static FunSigNode* createFunSig(IdNode* fullId, FuncParamsNode* params);
    string getFuncSignature() const;

    FunSigNode* copy();

    string toDot() const override;
    string getDotLabel() const override;
    list<Node *> getChildren() const override;
};


#endif //SCALA_LEXER_FUNSIGNODE_H
