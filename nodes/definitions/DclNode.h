#ifndef SCALA_LEXER_DCLNODE_H
#define SCALA_LEXER_DCLNODE_H


#include "../Node.h"
#include "../id/IdsNode.h"
#include "../type/CompoundTypeNode.h"
#include "../func/FunSigNode.h"
#include "../modifier/ModifiersNode.h"

class DclNode: public Node {
public:
    StatType type;
    IdsNode* ids;
    CompoundTypeNode* compoundType;
    FunSigNode* funSig;
    ModifiersNode* modifiers;

    DclNode();

    static DclNode* createVarDcl(IdsNode* ids, CompoundTypeNode* compoundType);
    static DclNode* createValDcl(IdsNode* ids, CompoundTypeNode* compoundType);
    static DclNode* createDefDcl(FunSigNode* funSig, CompoundTypeNode* compoundType);

    DclNode* copy();
    bool containsVar(string name);

    DclNode* setModifiers(ModifiersNode* modifiers);

    string toDot() const override;
    string getDotLabel() const override;
    list<Node *> getChildren() const override;

    void validateModifiers();
};


#endif //SCALA_LEXER_DCLNODE_H
