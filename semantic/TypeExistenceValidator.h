//
// TypeExistenceValidator - проверяет существование всех используемых типов
// Должен вызываться ПОСЛЕ LocalVarGatherVisitor и linkInheritanceHierarchy
//

#ifndef SCALA_LEXER_TYPEEXISTENCEVALIDATOR_H
#define SCALA_LEXER_TYPEEXISTENCEVALIDATOR_H

#include "nodes/Node.h"
#include "semantic/SemanticContext.h"
#include "semantic/tools/datatype.h"

class TopStatSeqNode;
class ClassDefNode;
class FunDefNode;
class VarDefsNode;
class SimpleTypeNode;
class SimpleExprNode;
class FuncParamNode;
class ClassParamNode;

class TypeExistenceValidator {
private:
    void visit(Node* node);
    void visitChildren(Node* node);

    void visitClassDef(ClassDefNode* node);
    void visitFunDef(FunDefNode* node);
    void visitVarDefs(VarDefsNode* node);
    void visitSimpleExpr(SimpleExprNode* node);
    void visitFuncParam(FuncParamNode* node);
    void visitClassParam(ClassParamNode* node);

    /**
     * Проверяет существование типа.
     * Примитивы и Array всегда валидны, для классов проверяет наличие в ctx().classes
     */
    void validateType(SimpleTypeNode* typeNode);

    /**
     * Проверяет существование типа по имени класса.
     * Используется для проверки типов при создании экземпляров (new ClassName)
     */
    void validateClassName(const std::string& className, int line);

public:
    TypeExistenceValidator() = default;
    void visitTree(TopStatSeqNode* root);
};

#endif //SCALA_LEXER_TYPEEXISTENCEVALIDATOR_H
