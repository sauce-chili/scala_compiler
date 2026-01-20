#ifndef SCALA_LEXER_TEMPLATEDEFNODE_H
#define SCALA_LEXER_TEMPLATEDEFNODE_H

#include "../Node.h"

class ClassDefNode;
class ClassTemplateOptNode;
class IdNode;


class TemplateDefNode: public Node {
public:
    StatType type;
    ClassDefNode* classDef;

    TemplateDefNode();

    static TemplateDefNode* createClassDef(ClassDefNode* classDef);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_TEMPLATEDEFNODE_H
