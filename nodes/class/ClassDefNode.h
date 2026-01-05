#ifndef SCALA_LEXER_CLASSDEFNODE_H
#define SCALA_LEXER_CLASSDEFNODE_H

#include "../Node.h"
#include "../id/IdNode.h"
#include "../modifier/ModifierNode.h"
#include "../class/ClassParamNodes.h"
#include "../templates/ClassTemplateOptNode.h"

class ClassDefNode: public Node {
public:
    IdNode* id;
    ModifierNode* modifier;
    ClassParamNodes* classParams;
    ClassTemplateOptNode* classTemplateOpt;

    ClassDefNode();

    static ClassDefNode* createClassDef(
            IdNode* id,
            ModifierNode* modifier,
            ClassParamNodes* classParams,
            ClassTemplateOptNode* classTemplateOpt
            );

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_CLASSDEFNODE_H
