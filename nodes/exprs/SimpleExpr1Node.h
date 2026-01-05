#ifndef SCALA_LEXER_SIMPLEEXPR1NODE_H
#define SCALA_LEXER_SIMPLEEXPR1NODE_H

#include "../Node.h"
#include "nodes/Types.h"
#include "ArgumentExprsNode.h"
#include "SimpleExprNode.h"
#include "../id/IdNode.h"

#include "../Node.h"
#include "ExprNode.h"

class SimpleExpr1Node: public Node {
public:
    SimpleExpr1Type type;
    int intValue;
    char charValue;
    double doubleValue;
    string stringValue;
    bool boolValue;
    SimpleExprNode* simpleExpr;
    ExprNode* expr;
    ArgumentExprsNode* argumentExprs;
    SimpleExpr1Node* simpleExpr1;
    IdNode* identifier;

    SimpleExpr1Node();
    SimpleExpr1Node* clone() const;

    static SimpleExpr1Node* createIntNode(int value);
    static SimpleExpr1Node* createCharNode(char value);
    static SimpleExpr1Node* createDoubleNode(double value);
    static SimpleExpr1Node* createStringNode(string value);
    static SimpleExpr1Node* createBoolNode(bool value);
    static SimpleExpr1Node* createNullNode();
    static SimpleExpr1Node* createIdNode(IdNode* identifier);
    static SimpleExpr1Node* createMethodCallNode(SimpleExpr1Node* left, ArgumentExprsNode* right);
    static SimpleExpr1Node* createSuperFieldAccessNode(IdNode* identifier);
    static SimpleExpr1Node* createThisFieldAccessNode(IdNode* identifier);
    static SimpleExpr1Node* createSimpleExprFieldAccessNode(IdNode* identifier, SimpleExprNode* expr);
    static SimpleExpr1Node* createArgumentCallNode(ExprNode* expr);
    static SimpleExpr1Node* createEmptyCallNode();

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_SIMPLEEXPR1NODE_H
