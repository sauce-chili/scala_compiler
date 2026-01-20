#ifndef SCALA_LEXER_DCLNODE_H
#define SCALA_LEXER_DCLNODE_H


#include "../Node.h"
#include "../id/IdsNode.h"
#include "../func/FunSigNode.h"

class DclNode: public Node {
public:
    StatType type;
    IdsNode* ids;
    SimpleTypeNode* simpleType;
    FunSigNode* funSig;

    DclNode();

    static DclNode* createVarDcl(IdsNode* ids, SimpleTypeNode* simpleType);
    static DclNode* createValDcl(IdsNode* ids, SimpleTypeNode* simpleType);
    static DclNode* createDefDcl(FunSigNode* funSig, SimpleTypeNode* simpleType);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_DCLNODE_H
