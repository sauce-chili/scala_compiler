#ifndef SCALA_LEXER_CLASSTEMPLATEOPTNODE_H
#define SCALA_LEXER_CLASSTEMPLATEOPTNODE_H


#include "../Node.h"
#include "../class/ExtensionClassTemplateNode.h"

class ClassTemplateOptNode: public Node {
public:
    ExtensionClassTemplateNode* classTemplate;
    TemplateStatsNode* templateStats;

    ClassTemplateOptNode();

    static ClassTemplateOptNode* addFuncParamToBackToList(ExtensionClassTemplateNode* classTemplate, TemplateStatsNode* templateStats);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_CLASSTEMPLATEOPTNODE_H
