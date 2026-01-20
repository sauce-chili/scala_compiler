#ifndef SCALA_LEXER_CLASSPARAMNODE_H
#define SCALA_LEXER_CLASSPARAMNODE_H


#include "../Node.h"
#include "../modifier/ModifiersNode.h"
#include "../id/IdNode.h"
#include "../type/SimpleTypeNode.h"

class ClassParamNode: public Node {
public:
    ClassParamType type;
    ModifiersNode* modifiers;
    IdNode* fullId;
    SimpleTypeNode* simpleType;

    ClassParamNode();

    static ClassParamNode* createClassParam(
            ClassParamType type,
            ModifiersNode* modifiers,
            IdNode* fullId,
            SimpleTypeNode* simpleType
    );

    ClassParamNode* copy();

    string toDot() const override;
    string getDotLabel() const override;
    list<Node *> getChildren() const override;
};


#endif //SCALA_LEXER_CLASSPARAMNODE_H
