#ifndef SCALA_LEXER_IDNODE_H
#define SCALA_LEXER_IDNODE_H

#include <string>
#include "../Node.h"

class IdNode: public Node {
public:
    string name;

    IdNode* clone() const;

    static IdNode* createId(string name);

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_IDNODE_H
