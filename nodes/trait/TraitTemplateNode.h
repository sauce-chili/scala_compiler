#ifndef SCALA_LEXER_TRAITTEMPLATENODE_H
#define SCALA_LEXER_TRAITTEMPLATENODE_H


#include "../Node.h"

class TemplateStatsNode;
class SimpleTypeNode;
class SimpleTypesNode;

class TraitTemplateNode: public Node {
public:
    SimpleTypeNode* simpleType;
    SimpleTypesNode* simpleTypes;
    TemplateStatsNode* templateStats;

    TraitTemplateNode();

    static TraitTemplateNode* createTraitTemplate(SimpleTypeNode* simpleType, SimpleTypesNode* simpleTypes, TemplateStatsNode* templateStats);

    TraitTemplateNode* copy();

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_TRAITTEMPLATENODE_H
