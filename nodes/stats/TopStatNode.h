#ifndef SCALA_LEXER_TOPSTATNODE_H
#define SCALA_LEXER_TOPSTATNODE_H


#include "../Node.h"
#include "../modifier/ModifiersNode.h"
#include "../class/ClassDefNode.h"

class TopStatNode: public Node {
public:
    ModifiersNode* modifiers;
    ClassDefNode* classDef;

    TopStatNode();

    static TopStatNode* createClass(ModifiersNode* modifiers, ClassDefNode* classDef);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_TOPSTATNODE_H
