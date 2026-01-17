#ifndef SCALA_LEXER_BLOCKSTATNODE_H
#define SCALA_LEXER_BLOCKSTATNODE_H


#include "../Node.h"

class VarDefsNode;
class ExprNode;

class BlockStatNode: public Node {
public:
    VarDefsNode* varDefs;
    ExprNode* expr;

    BlockStatNode();

    static BlockStatNode* createVarDefsNode(VarDefsNode* varDefs);
    static BlockStatNode* createExprNode(ExprNode* expr);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_BLOCKSTATNODE_H
