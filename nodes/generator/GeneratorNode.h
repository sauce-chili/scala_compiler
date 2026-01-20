#ifndef SCALA_LEXER_GENERATORNODE_H
#define SCALA_LEXER_GENERATORNODE_H


#include "../Node.h"
#include "../id/IdNode.h"
#include "../exprs/ExprNode.h"
#include "../type/SimpleTypeNode.h"

class GeneratorNode: public Node {
public:
    IdNode* fullId;
    SimpleTypeNode* simpleType;
    ExprNode* expr;

    GeneratorNode();

    static GeneratorNode* createGenerator(IdNode* fullId, SimpleTypeNode* simpleType, ExprNode* expr);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_GENERATORNODE_H
