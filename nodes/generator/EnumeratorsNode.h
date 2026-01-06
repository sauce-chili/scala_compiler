#ifndef SCALA_LEXER_ENUMERATORSNODE_H
#define SCALA_LEXER_ENUMERATORSNODE_H

#include "../Node.h"
#include "GeneratorNode.h"
#include "EnumeratorPartNode.h"

class EnumeratorsNode: public Node {
public:
    GeneratorNode* generator;
    std::list<EnumeratorPartNode*> *enumerators;

    EnumeratorsNode(GeneratorNode* generator);

    static EnumeratorsNode* addModifierToList(EnumeratorsNode *list, EnumeratorPartNode *enumeratorPart);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_ENUMERATORSNODE_H
