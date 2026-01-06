#ifndef SCALA_LEXER_ENUMCASENODE_H
#define SCALA_LEXER_ENUMCASENODE_H


#include "nodes/Node.h"
#include "nodes/id/IdsNode.h"
#include "nodes/class/ClassParamsNode.h"
#include "nodes/class/ClassParentsNode.h"

class EnumCaseNode: public Node {
public:
    EnumCaseType type;
    IdsNode* ids;
    ClassParamsNode* classParams;
    ClassParentsNode* classParents;

    EnumCaseNode();

    static EnumCaseNode* createClassParents(EnumCaseType type, IdsNode* ids, ClassParamsNode* classParams, ClassParentsNode* classParents);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_ENUMCASENODE_H
