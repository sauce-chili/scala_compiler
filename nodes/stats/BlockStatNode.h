#ifndef SCALA_LEXER_BLOCKSTATNODE_H
#define SCALA_LEXER_BLOCKSTATNODE_H


#include "../Node.h"

class VarDefsNode;
class ExprNode;
class SimpleExpr1Node;
class SimpleExprNode;

class BlockStatNode: public Node {
public:
    VarDefsNode* varDefs;
    ExprNode* expr;

    BlockStatNode();

    static BlockStatNode* createVarDefsNode(VarDefsNode* varDefs);
    static BlockStatNode* createExprNode(ExprNode* expr);

    static BlockStatNode* createSimpleExpr1(SimpleExpr1Node* simpleExpr1);
    static ExprNode* createExpr(SimpleExprNode* simpleExpr);

    BlockStatNode* copy();

    string toDot() const override;
    string getDotLabel() const override;
    list<Node *> getChildren() const override;
};


#endif //SCALA_LEXER_BLOCKSTATNODE_H
