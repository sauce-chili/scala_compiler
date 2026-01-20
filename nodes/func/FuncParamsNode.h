#ifndef SCALA_LEXER_FUNCPARAMSNODE_H
#define SCALA_LEXER_FUNCPARAMSNODE_H


#include "../Node.h"
#include "FuncParamNode.h"

#include <list>

class FuncParamsNode: public Node {
public:
    std::list<FuncParamNode*> *funcParams;

    FuncParamsNode();
    FuncParamsNode(FuncParamNode* first);

    static FuncParamsNode* addFuncParamToList(FuncParamsNode *list, FuncParamNode *funcParam);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_FUNCPARAMSNODE_H
