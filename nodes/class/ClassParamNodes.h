#ifndef SCALA_LEXER_CLASSPARAMNODES_H
#define SCALA_LEXER_CLASSPARAMNODES_H


#include "../Node.h"
#include "ClassParamNode.h"

class ClassParamNodes: public Node {
public:
    std::list<ClassParamNode*> *classParams;

    ClassParamNodes();
    ClassParamNodes(ClassParamNode *first);

    static ClassParamNodes* addClassParamToList(ClassParamNodes *list, ClassParamNode *classParam);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_CLASSPARAMNODES_H
