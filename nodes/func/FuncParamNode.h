#ifndef SCALA_LEXER_FUNCPARAMNODE_H
#define SCALA_LEXER_FUNCPARAMNODE_H


#include "../Node.h"
#include "nodes/id/IdNode.h"
#include "nodes/type/CompoundTypeNode.h"

class FuncParamNode: public Node {
public:
    IdNode* fullId;
    CompoundTypeNode* compoundType;

    FuncParamNode();

    static FuncParamNode* createFuncParam(IdNode* fullId, CompoundTypeNode* compoundType);

    FuncParamNode* copy();

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_FUNCPARAMNODE_H
