#ifndef SCALA_LEXER_DEFNODE_H
#define SCALA_LEXER_DEFNODE_H


#include "../Node.h"
#include "../var/VarDefsNode.h"
#include "../func/FunDefNode.h"

class DefNode: public Node {
public:
    DefinitionType type;
    VarDefsNode* varDefs;
    FunDefNode* funDef;

    DefNode();

    static DefNode* createVarDefs(VarDefsNode* varDefs);
    static DefNode* createFunDef(FunDefNode* funDef);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_DEFNODE_H
