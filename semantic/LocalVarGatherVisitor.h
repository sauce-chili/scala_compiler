//
// LocalVarGatherVisitor - собирает локальные переменные в таблицы методов
// Должен вызываться ПОСЛЕ ScopeAttachVisitor (чтобы скоупы были расставлены)
//

#ifndef SCALA_LEXER_LOCALVARGATHERVISITOR_H
#define SCALA_LEXER_LOCALVARGATHERVISITOR_H

#include "nodes/Node.h"
#include "semantic/SemanticContext.h"
#include "semantic/scopes/Scope.h"
#include "semantic/tables/tables.hpp"
#include "semantic/tools/datatype.h"

class TopStatSeqNode;
class ClassDefNode;
class FunDefNode;
class BlockStatsNode;
class BlockStatNode;
class VarDefsNode;
class EnumeratorsNode;
class GeneratorNode;
class ExprNode;

class LocalVarGatherVisitor {
private:
    ClassMetaInfo* currentClass = nullptr;
    MethodMetaInfo* currentMethod = nullptr;
    Scope* currentScope = nullptr;

    void visit(Node* node);
    void visitChildren(Node* node);

    void visitClassDef(ClassDefNode* node);
    void visitFunDef(FunDefNode* node);
    void visitBlockStats(BlockStatsNode* node);
    void visitBlockStat(BlockStatNode* node);
    void visitVarDefs(VarDefsNode* node, Scope* scope);
    void visitForExpression(ExprNode* node);
    void visitGenerator(GeneratorNode* node, Scope* scope);

public:
    LocalVarGatherVisitor() = default;
    void visitTree(TopStatSeqNode* root);
};

#endif //SCALA_LEXER_LOCALVARGATHERVISITOR_H
