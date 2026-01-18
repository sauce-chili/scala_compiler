#ifndef SCALA_COMPILER_MODIFIERLIST_H
#define SCALA_COMPILER_MODIFIERLIST_H

#include "../Node.h"
#include "../types.h"

class ModifierNode : public Node {
public:
    ModifierType type;

    static ModifierNode* createModifier(ModifierType type);

    ModifierNode *copy();

    string toDot() const override;
    string getDotLabel() const override;


    bool isAccessModifier() const;
    bool isOverrideModifier() const;
    bool isInheritModifier() const;
};


#endif 