#ifndef SCALA_LEXER_CLASSPARAMSNODE_H
#define SCALA_LEXER_CLASSPARAMSNODE_H


#include "../Node.h"
#include "ClassParamNode.h"

class ClassParamsNode: public Node {
public:
    std::list<ClassParamNode*> *classParams;

    ClassParamsNode();
    ClassParamsNode(ClassParamNode *first);

    ClassParamsNode* copy();

    static ClassParamsNode* addClassParamToList(ClassParamsNode *list, ClassParamNode *classParam);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_CLASSPARAMSNODE_H
