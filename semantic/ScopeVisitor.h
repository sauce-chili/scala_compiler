//
// Created by Dmitry Smirnov on 15.01.2026.
//

#ifndef SCALA_LEXER_SCOPEVISITOR_H
#define SCALA_LEXER_SCOPEVISITOR_H
#include "scopes/Scope.h"


class TopStatSeqNode;
class BlockStatsNode;
class FunDefNode;
class FuncParamsNode;
class Node;
class TemplateDefNode;

class ScopeVisitor {
    Scope *currentScope;

    void visit(Node *node);

    void visitChildren(Node *node);

    void handleClassScope(TemplateDefNode *node);

    void handleFuncParamsScope(FuncParamsNode *node);

    void handleFunDef(FunDefNode *node);

    void handleBlockScope(BlockStatsNode *node);

public:
    ScopeVisitor();

    void visitTree(TopStatSeqNode *topStatSeq);
};


#endif //SCALA_LEXER_SCOPEVISITOR_H
