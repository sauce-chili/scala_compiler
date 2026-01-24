#ifndef SCALA_LEXER_DCLNODE_H
#define SCALA_LEXER_DCLNODE_H


#include "../Node.h"
#include "../func/FunSigNode.h"
#include "../modifier/ModifiersNode.h"

class DclNode: public Node {
public:
    StatType type;
    IdNode* fullId;
    SimpleTypeNode* simpleType;
    FunSigNode* funSig;
    ModifiersNode* modifiers;

    DclNode();

    static DclNode* createVarDcl(IdNode* fullId, SimpleTypeNode* simpleType);
    static DclNode* createValDcl(IdNode* fullId, SimpleTypeNode* simpleType);
    static DclNode* createDefDcl(FunSigNode* funSig, SimpleTypeNode* simpleType);

    DclNode* copy();
    bool containsVar(string name);

    DclNode* setModifiers(ModifiersNode* modifiers);

    string toDot() const override;
    string getDotLabel() const override;
    list<Node *> getChildren() const override;

    void validateModifiers();
};


#endif //SCALA_LEXER_DCLNODE_H
