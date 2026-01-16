#ifndef SCALA_LEXER_TEMPLATEDEFNODE_H
#define SCALA_LEXER_TEMPLATEDEFNODE_H

#include "../Node.h"

class ClassDefNode;
class ClassTemplateOptNode;
class IdNode;
class TraitTemplateOptNode;
class EnumDefNode;


class TemplateDefNode: public Node {
public:
    StatType type;
    ClassDefNode* classDef;
    IdNode* fullId;
    ClassTemplateOptNode* classTemplateOpt;
    TraitTemplateOptNode* traitTemplateOpt;
    EnumDefNode* enumDef;
    Scope* classScope; // тип класс, сразу назначать имя

    TemplateDefNode();

    static TemplateDefNode* createClassDef(ClassDefNode* classDef);
    static TemplateDefNode* createObjectDef(IdNode* fullId, ClassTemplateOptNode* classTemplateOpt);
    static TemplateDefNode* createTraitDef(IdNode* fullId, TraitTemplateOptNode* traitTemplateOpt);
    static TemplateDefNode* createEnumDef(EnumDefNode* enumDef);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_TEMPLATEDEFNODE_H
