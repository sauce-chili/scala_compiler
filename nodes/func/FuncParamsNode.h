#ifndef SCALA_LEXER_FUNCPARAMSNODE_H
#define SCALA_LEXER_FUNCPARAMSNODE_H


#include "../Node.h"
#include "FuncParamNode.h"
#include "semantic/scopes/Scope.h"
#include "../class/ClassParamsNode.h"

#include <list>

class FuncParamsNode: public Node {
public:
    std::list<FuncParamNode *> *funcParams;

    FuncParamsNode();

    FuncParamsNode(FuncParamNode *first);
    FuncParamsNode(ClassParamsNode *first);

    static FuncParamsNode *addFuncParamToList(FuncParamsNode *list, FuncParamNode *funcParam);

    FuncParamsNode* copy();

    string toDot() const override;

    string getDotLabel() const override;

    list<Node *> getChildren() const override;
};


#endif //SCALA_LEXER_FUNCPARAMSNODE_H
