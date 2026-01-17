#ifndef SCALA_LEXER_MODIFIERSNODE_H
#define SCALA_LEXER_MODIFIERSNODE_H

#include <list>

#include "../Node.h"
#include "ModifierNode.h"

class ModifiersNode: public Node {
public:
    std::list<ModifierNode*> *modifiers;

    ModifiersNode();
    ModifiersNode(ModifierNode* modifier);

    ModifiersNode* copy();

    static ModifiersNode* addModifierToList(ModifiersNode *list, ModifierNode *modifier);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_MODIFIERSNODE_H
