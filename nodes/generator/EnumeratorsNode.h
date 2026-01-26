#ifndef SCALA_LEXER_ENUMERATORSNODE_H
#define SCALA_LEXER_ENUMERATORSNODE_H

#include "../Node.h"
#include "GeneratorNode.h"

class Scope;

class EnumeratorsNode: public Node {
public:
    GeneratorNode* generator;
    Scope* scope = nullptr;

    EnumeratorsNode(GeneratorNode* generator);
    EnumeratorsNode();

    EnumeratorsNode* copy();

    string toDot() const override;
    string getDotLabel() const override;
    list<Node *> getChildren() const override;
};


#endif //SCALA_LEXER_ENUMERATORSNODE_H
