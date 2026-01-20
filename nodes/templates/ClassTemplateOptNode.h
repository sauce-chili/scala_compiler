#ifndef SCALA_LEXER_CLASSTEMPLATEOPTNODE_H
#define SCALA_LEXER_CLASSTEMPLATEOPTNODE_H


#include "../Node.h"
#include "../class/ExtensionClassTemplateNode.h"

class ClassTemplateOptNode: public Node {
public:
    ExtensionClassTemplateNode* classTemplate;
    TemplateStatsNode* templateStats;
    ModifiersNode* modifiers;

    ClassTemplateOptNode();

    static ClassTemplateOptNode* addFuncParamToBackToList(ExtensionClassTemplateNode* classTemplate, TemplateStatsNode* templateStats);

    ClassTemplateOptNode* copy();
    ClassTemplateOptNode* setModifiers(ModifiersNode* modifiers);

    string toDot() const override;
    string getDotLabel() const override;
    list<Node *> getChildren() const override;
};


#endif //SCALA_LEXER_CLASSTEMPLATEOPTNODE_H
