#ifndef SCALA_LEXER_ENUMERATORPARTNODE_H
#define SCALA_LEXER_ENUMERATORPARTNODE_H


#include "../Node.h"
#include "GeneratorNode.h"

class EnumeratorPartNode: public Node {
public:
    GeneratorNode* generator;
    IdNode* fullId;
    SimpleTypeNode* simpleType;
    ExprNode* expr;

    EnumeratorPartNode();

    static EnumeratorPartNode* createGeneratorEnumeratorPart(GeneratorNode* generator);
    static EnumeratorPartNode* createVarDefEnumeratorPart(IdNode* fullId, SimpleTypeNode* simpleType, ExprNode* expr);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_ENUMERATORPARTNODE_H
