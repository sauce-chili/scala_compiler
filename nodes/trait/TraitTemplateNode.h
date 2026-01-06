#ifndef SCALA_LEXER_TRAITTEMPLATENODE_H
#define SCALA_LEXER_TRAITTEMPLATENODE_H


#include "../Node.h"
#include "../type/SimpleTypesNode.h"
#include "../templates/TemplateStatsNode.h"

class TraitTemplateNode: public Node {
public:
    SimpleTypeNode* simpleType;
    SimpleTypesNode* simpleTypes;
    TemplateStatsNode* templateStats;

    TraitTemplateNode();

    static TraitTemplateNode* createTraitTemplate(SimpleTypeNode* simpleType, SimpleTypesNode* simpleTypes, TemplateStatsNode* templateStats);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_TRAITTEMPLATENODE_H
