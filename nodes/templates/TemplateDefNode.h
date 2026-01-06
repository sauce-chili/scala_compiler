#ifndef SCALA_LEXER_TEMPLATEDEFNODE_H
#define SCALA_LEXER_TEMPLATEDEFNODE_H

#include "../Node.h"
#include "../class/ClassDefNode.h"
#include "../trait/TraitTemplateOptNode.h"
#include "../enum/EnumDefNode.h"

class TemplateDefNode: public Node {
public:
    StatType type;
    ClassDefNode* classDef;
    IdNode* id;
    ClassTemplateOptNode* classTemplateOpt;
    TraitTemplateOptNode* traitTemplateOpt;
    EnumDefNode* enumDef;

    TemplateDefNode();

    static TemplateDefNode* createClassDef(ClassDefNode* classDef);
    static TemplateDefNode* createObjectDef(IdNode* id, ClassTemplateOptNode* classTemplateOpt);
    static TemplateDefNode* createTraitDef(IdNode* id, TraitTemplateOptNode* traitTemplateOpt);
    static TemplateDefNode* createEnumDef(EnumDefNode* enumDef);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_TEMPLATEDEFNODE_H
