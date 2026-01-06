#ifndef SCALA_LEXER_GENERATORNODE_H
#define SCALA_LEXER_GENERATORNODE_H


#include "../Node.h"
#include "../id/IdNode.h"
#include "../type/CompoundTypeNode.h"
#include "../exprs/ExprNode.h"

class GeneratorNode: public Node {
public:
    IdNode* fullId;
    CompoundTypeNode* compoundType;
    ExprNode* expr;

    GeneratorNode();

    static GeneratorNode* createGenerator(IdNode* fullId, CompoundTypeNode* compoundType, ExprNode* expr);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_GENERATORNODE_H
