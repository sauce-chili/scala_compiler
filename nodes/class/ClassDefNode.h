#ifndef SCALA_LEXER_CLASSDEFNODE_H
#define SCALA_LEXER_CLASSDEFNODE_H

#include "../Node.h"
#include "../id/IdNode.h"
#include "../modifier/ModifierNode.h"
#include "../class/ClassParamsNode.h"
#include "../templates/ClassTemplateOptNode.h"

class ClassDefNode: public Node {
public:
    IdNode* fullId;
    ModifierNode* primaryConstructModifier; // TODO пофиксить
    ClassParamsNode* classParams;
    ClassTemplateOptNode* classTemplateOpt;

    ClassDefNode();

    ClassDefNode* copy();

    static ClassDefNode* createClassDef(
            IdNode* fullId,
            ModifierNode* modifier,
            ClassParamsNode* classParams,
            ClassTemplateOptNode* classTemplateOpt
            );

    string toDot() const override;
    string getDotLabel() const override;

    list<Node *> getChildren() const override;
};


#endif //SCALA_LEXER_CLASSDEFNODE_H
