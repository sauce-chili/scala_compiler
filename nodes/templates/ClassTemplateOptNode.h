#ifndef SCALA_LEXER_CLASSTEMPLATEOPTNODE_H
#define SCALA_LEXER_CLASSTEMPLATEOPTNODE_H


#include "../Node.h"
#include "../class/ExtensionClassTemplateNode.h"

class ClassTemplateOptNode: public Node {
public:
    ExtensionClassTemplateNode* extensionPartClassTemplate;
    TemplateStatsNode* templateStats;
    ModifiersNode* modifiers;

    ClassTemplateOptNode();

    static ClassTemplateOptNode* addFuncParamToBackToList(ExtensionClassTemplateNode* classTemplate, TemplateStatsNode* templateStats);
    static ClassTemplateOptNode* createClassWithoutInherit(TemplateStatsNode* templateStats);

    ClassTemplateOptNode* copy();
    ClassTemplateOptNode* setModifiers(ModifiersNode* modifiers);

    void processConsole();

    string toDot() const override;
    string getDotLabel() const override;
    list<Node *> getChildren() const override;
};


#endif //SCALA_LEXER_CLASSTEMPLATEOPTNODE_H
