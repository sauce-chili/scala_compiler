#ifndef SCALA_LEXER_DEFNODE_H
#define SCALA_LEXER_DEFNODE_H


#include "../Node.h"
#include "../var/VarDefsNode.h"
#include "../func/FunDefNode.h"
#include "../templates/TemplateDefNode.h"
#include "../modifier/ModifiersNode.h"
#include "../class/PrimaryConstructorNode.h"

class DefNode: public Node {
public:
    DefinitionType type;
    VarDefsNode* varDefs;
    FunDefNode* funDef;
    TemplateDefNode* tmplDef;
    ModifiersNode* modifiers;
    /**
     * Искусственный узел
     */
    PrimaryConstructorNode* primaryConstructor;

    DefNode();

    DefNode* copy();

    static DefNode* createVarDefs(VarDefsNode* varDefs);
    static DefNode* createFunDef(FunDefNode* funDef);
    static DefNode* createTmplDef(TemplateDefNode* tmplDef);

    static DefNode* createPrimaryConstructor(PrimaryConstructorNode* primaryConstructor);

    DefNode* setModifiers(ModifiersNode* modifiers);
    bool containsVar(string name);

    string toDot() const override;
    string getDotLabel() const override;
    list<Node *> getChildren() const override;
};


#endif //SCALA_LEXER_DEFNODE_H
