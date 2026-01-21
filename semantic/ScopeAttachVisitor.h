//
// Created by Dmitry Smirnov on 15.01.2026.
//

#ifndef SCALA_LEXER_SCOPEVISITOR_H
#define SCALA_LEXER_SCOPEVISITOR_H
#include "nodes/exprs/ExprNode.h"
#include "scopes/Scope.h"


class TopStatSeqNode;
class BlockStatsNode;
class FunDefNode;
class FuncParamsNode;
class Node;
class TemplateDefNode;

class ScopeAttachVisitor {
    Scope *currentScope;

    void visit(Node *node);

    void visitChildren(Node *node);

    void handleFunDef(FunDefNode *node);

    void handleBlockStats(BlockStatsNode *node);

    void handleForExpression(ExprNode *node);

public:
    ScopeAttachVisitor();

    void visitTree(TopStatSeqNode *topStatSeq);
};


#endif //SCALA_LEXER_SCOPEVISITOR_H
