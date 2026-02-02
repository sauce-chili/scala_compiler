#ifndef SCALA_LEXER_ARGUMENTEXPRSNODE_H
#define SCALA_LEXER_ARGUMENTEXPRSNODE_H

#include "../Node.h"
#include "ExprNode.h"
#include "ExprsNode.h"
#include "semantic/tools/datatype.h"
#include <list>
#include <vector>

class ClassMetaInfo;
class MethodMetaInfo;
class Scope;

class ArgumentExprsNode: public Node {
public:
    ExprsNode* exprs;

    ArgumentExprsNode();
    ArgumentExprsNode(ExprsNode* exprs);

    ArgumentExprsNode* copy();

    bool contains(string name);

    std::vector<DataType*> getArgsTypes(
        ClassMetaInfo* currentClass,
        MethodMetaInfo* currentMethod,
        Scope* currentScope
    ) const;

    string toDot() const override;
    string getDotLabel() const override;
    list<Node *> getChildren() const override;
};


#endif //SCALA_LEXER_ARGUMENTEXPRSNODE_H
