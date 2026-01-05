#ifndef SCALA_LEXER_CLASSPARAMNODE_H
#define SCALA_LEXER_CLASSPARAMNODE_H


#include "../Node.h"
#include "../modifier/ModifiersNode.h"
#include "../id/IdNode.h"
#include "../type/CompoundTypeNode.h"
#include "../exprs/AssignExprNode.h"

class ClassParamNode: public Node {
public:
    ClassParamType type;
    ModifiersNode* modifiers;
    IdNode* id;
    CompoundTypeNode* compoundType;
    AssignExprNode* assignExpr;

    ClassParamNode();

    static ClassParamNode* createClassParam(
            ClassParamType type,
            ModifiersNode* modifiers,
            IdNode* id,
            CompoundTypeNode* compoundType,
            AssignExprNode* assignExpr
    );

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_CLASSPARAMNODE_H
