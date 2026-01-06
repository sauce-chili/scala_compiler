#ifndef SCALA_LEXER_TRAITTEMPLATEOPTNODE_H
#define SCALA_LEXER_TRAITTEMPLATEOPTNODE_H


#include "../Node.h"
#include "TraitTemplateNode.h"

class TraitTemplateOptNode: public Node {
public:
    TraitTemplateNode* traitTemplate;
    TemplateStatsNode* templateStats;

    TraitTemplateOptNode();

    static TraitTemplateOptNode* createTraitTemplateOpt(TraitTemplateNode* traitTemplate, TemplateStatsNode* templateStats);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_TRAITTEMPLATEOPTNODE_H
