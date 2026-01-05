#ifndef SCALA_LEXER_DCLNODE_H
#define SCALA_LEXER_DCLNODE_H


#include "../Node.h"
#include "../id/IdsNode.h"
#include "../type/CompoundTypeNode.h"
#include "../func/FunSigNode.h"

class DclNode: public Node {
public:
    StatType type;
    IdsNode* ids;
    CompoundTypeNode* compoundType;
    FunSigNode* funSig;

    DclNode();

    static DclNode* createVarDcl(IdsNode* ids, CompoundTypeNode* compoundType);
    static DclNode* createValDcl(IdsNode* ids, CompoundTypeNode* compoundType);
    static DclNode* createDefDcl(FunSigNode* funSig, CompoundTypeNode* compoundType);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_DCLNODE_H
