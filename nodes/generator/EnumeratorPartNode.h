#ifndef SCALA_LEXER_ENUMERATORPARTNODE_H
#define SCALA_LEXER_ENUMERATORPARTNODE_H


#include "../Node.h"
#include "GeneratorNode.h"

class EnumeratorPartNode: public Node {
public:
    GeneratorNode* generator;
    IdNode* fullId;
    CompoundTypeNode* compoundType;
    ExprNode* expr;

    EnumeratorPartNode();

    EnumeratorPartNode* copy();

    static EnumeratorPartNode* createGeneratorEnumeratorPart(GeneratorNode* generator);
    static EnumeratorPartNode* createVarDefEnumeratorPart(IdNode* fullId, CompoundTypeNode* compoundType, ExprNode* expr);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_ENUMERATORPARTNODE_H
