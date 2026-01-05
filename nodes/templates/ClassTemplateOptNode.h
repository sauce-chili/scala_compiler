#ifndef SCALA_LEXER_CLASSTEMPLATEOPTNODE_H
#define SCALA_LEXER_CLASSTEMPLATEOPTNODE_H


#include "../Node.h"
#include "../class/ClassTemplateNode.h"

class ClassTemplateOptNode: public Node {
public:
    ClassTemplateNode* classTemplate;
    TemplateStatsNode* templateStats;

    ClassTemplateOptNode();

    static ClassTemplateOptNode* addFuncParamToBackToList(ClassTemplateNode* classTemplate, TemplateStatsNode* templateStats);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_CLASSTEMPLATEOPTNODE_H
