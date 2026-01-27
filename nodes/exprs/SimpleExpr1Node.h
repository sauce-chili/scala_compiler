#ifndef SCALA_LEXER_SIMPLEEXPR1NODE_H
#define SCALA_LEXER_SIMPLEEXPR1NODE_H

#include "../Node.h"
#include "../Types.h"

class ArgumentExprsNode;
class SimpleExprNode;
class ExprNode;
class IdNode;
class SimpleTypeNode;

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
    SimpleTypeNode* simpleType;

    SimpleExpr1Node();

    SimpleExpr1Node* copy();

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
    static SimpleExpr1Node* createParenthesizedExprNode(ExprNode* expr);
    static SimpleExpr1Node* createUnitLiteralNode();
    static SimpleExpr1Node* createPlainThisNode();
    static SimpleExpr1Node* createArrayWithTypeBuilderNode(SimpleTypeNode* simpleType, ArgumentExprsNode* argumentExprs);
    static SimpleExpr1Node* createArrayBuilderNode(ArgumentExprsNode* argumentExprs);

    string toDot() const override;
    string getDotLabel() const override;
    list<Node *> getChildren() const override;
};


#endif //SCALA_LEXER_SIMPLEEXPR1NODE_H
