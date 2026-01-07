#ifndef SCALA_LEXER_IDNODE_H
#define SCALA_LEXER_IDNODE_H

#include <string>
#include "../Node.h"

class IdNode: public Node {
public:
    string name;

    static IdNode* createId(string name);
    static IdNode* createOperator(string op);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_IDNODE_H
