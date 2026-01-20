#ifndef SCALA_LEXER_FUNCPARAMNODE_H
#define SCALA_LEXER_FUNCPARAMNODE_H


#include "../Node.h"
#include "nodes/id/IdNode.h"
#include "../type/SimpleTypeNode.h"

class FuncParamNode: public Node {
public:
    IdNode* fullId;
    SimpleTypeNode* simpleType;

    FuncParamNode();

    static FuncParamNode* createFuncParam(IdNode* fullId, SimpleTypeNode* simpleType);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_FUNCPARAMNODE_H
