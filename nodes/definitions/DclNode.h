#ifndef SCALA_LEXER_DCLNODE_H
#define SCALA_LEXER_DCLNODE_H


#include "../Node.h"
#include "../id/IdsNode.h"
#include "../func/FunSigNode.h"
#include "../modifier/ModifiersNode.h"

class DclNode: public Node {
public:
    StatType type;
    IdsNode* ids;
    SimpleTypeNode* simpleType;
    FunSigNode* funSig;
    ModifiersNode* modifiers;

    DclNode();

    static DclNode* createVarDcl(IdsNode* ids, SimpleTypeNode* simpleType);
    static DclNode* createValDcl(IdsNode* ids, SimpleTypeNode* simpleType);
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
