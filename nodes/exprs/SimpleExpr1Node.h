#ifndef SCALA_LEXER_SIMPLEEXPR1NODE_H
#define SCALA_LEXER_SIMPLEEXPR1NODE_H

#include "../Node.h"
#include "nodes/Types.h"
#include "ArgumentExprsNode.h"

class SimpleExpr1Node: public Node {
public:
    SimpleExpr1Type type;
    ExprNode* left;
    ExprNode* right;
    int intValue;
    char charValue;
    double doubleValue;
    string stringValue;
    bool boolValue;
    string identifierName;
    ExprNode* elements;
    SimpleExprNode* simpleExpr;
    ExprNode* expr;
    ArgumentExprsNode* argumentExprs;
    SimpleExpr1Node* simpleExpr1;

    SimpleExpr1Node();
    SimpleExpr1Node* clone() const;

    static SimpleExpr1Node* createIntNode(int value);
    static SimpleExpr1Node* createCharNode(char value);
    static SimpleExpr1Node* createDoubleNode(double value);
    static SimpleExpr1Node* createStringNode(string value);
    static SimpleExpr1Node* createBoolNode(bool value);
    static SimpleExpr1Node* createNullNode();
    static ExprNode* createMethodCallNode(SimpleExpr1Node* left, ArgumentExprsNode* right);
    static ExprNode* createSuperFieldAccessNode(string fieldName);
    static ExprNode* createThisFieldAccessNode(string fieldName);
    static ExprNode* createSimpleExprAccessNode(string name, ExprNode* expr);
    static ExprNode* createArgumentCallNode(ExprNode* expr);
    static ExprNode* createEmptyCallNode(ExprNode* expr);
    static ExprNode* createMethodCallNode(string methodName, ArgumentExprsNode* arguments);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_SIMPLEEXPR1NODE_H
