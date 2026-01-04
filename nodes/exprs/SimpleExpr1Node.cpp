#include "SimpleExpr1Node.h"

SimpleExpr1Node::SimpleExpr1Node() {

}

SimpleExpr1Node* SimpleExpr1Node::clone() const {

}

SimpleExpr1Node* SimpleExpr1Node::createIntNode(int value) {

}

SimpleExpr1Node* SimpleExpr1Node::createCharNode(char value) {

}

SimpleExpr1Node* SimpleExpr1Node::createDoubleNode(double value) {

}

SimpleExpr1Node* SimpleExpr1Node::createStringNode(string value) {

}

SimpleExpr1Node* SimpleExpr1Node::createBoolNode(bool value) {

}

SimpleExpr1Node* SimpleExpr1Node::createNullNode() {

}

ExprNode* SimpleExpr1Node::createMethodCallNode(SimpleExpr1Node* left, ArgumentExprsNode* right) {

}

ExprNode* SimpleExpr1Node::createSuperFieldAccessNode(string fieldName) {

}

ExprNode* SimpleExpr1Node::createThisFieldAccessNode(string fieldName) {

}

ExprNode* SimpleExpr1Node::createSimpleExprAccessNode(string name, ExprNode* expr) {

}

ExprNode* SimpleExpr1Node::createArgumentCallNode(ExprNode* expr) {

}

ExprNode* SimpleExpr1Node::createEmptyCallNode(ExprNode* expr) {

}

ExprNode* SimpleExpr1Node::createMethodCallNode(string methodName, ArgumentExprsNode* arguments) {

}

string SimpleExpr1Node::toDot() const {

}

string SimpleExpr1Node::getDotLabel() const {

}