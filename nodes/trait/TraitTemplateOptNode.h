#ifndef SCALA_LEXER_TRAITTEMPLATEOPTNODE_H
#define SCALA_LEXER_TRAITTEMPLATEOPTNODE_H


#include "../Node.h"
#include "TraitTemplateNode.h"
#include "../modifier/ModifiersNode.h"

class TraitTemplateOptNode: public Node {
public:
    TraitTemplateNode* traitTemplate;
    TemplateStatsNode* templateStats;
    ModifiersNode* modifiers;

    TraitTemplateOptNode();

    static TraitTemplateOptNode* createTraitTemplateOpt(TraitTemplateNode* traitTemplate, TemplateStatsNode* templateStats);

    TraitTemplateOptNode* copy();
    TraitTemplateOptNode* setModifiers(ModifiersNode* modifiers);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_TRAITTEMPLATEOPTNODE_H
