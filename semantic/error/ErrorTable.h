#ifndef SCALA_LEXER_ERRORTABLE_H
#define SCALA_LEXER_ERRORTABLE_H

#include "SemanticError.h"
#include <list>

class ErrorTable {
public:
    static std::list<SemanticError*> *errors;

    ErrorTable();

    static void addErrorToList(SemanticError *error);

    static std::string getErrors();
};


#endif //SCALA_LEXER_ERRORTABLE_H
