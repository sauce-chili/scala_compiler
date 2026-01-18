#ifndef SCALA_LEXER_CLASSPARENTSNODE_H
#define SCALA_LEXER_CLASSPARENTSNODE_H


#include "../Node.h"
#include "ConstrInvokeNode.h"
#include "../type/SimpleTypesNode.h"

class ClassParentsNode: public Node {
public:
    ConstrInvokeNode* constr;
    SimpleTypesNode* simpleTypes;

    ClassParentsNode();

    ClassParentsNode* copy();

    static ClassParentsNode* createClassParents(ConstrInvokeNode* constr, SimpleTypesNode* simpleTypes);

    string toDot() const override;
    string getDotLabel() const override;
    list<Node *> getChildren() const override;
};


#endif //SCALA_LEXER_CLASSPARENTSNODE_H
