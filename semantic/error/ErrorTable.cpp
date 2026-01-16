#include "ErrorTable.h"

std::list<SemanticError*> ErrorTable::errors;

void ErrorTable::addErrorToList(SemanticError *error) {
    errors.push_back(error);
}

std::string ErrorTable::getErrors() {
    std::string messages = "";

    for (auto error: errors) {
        messages += error->getErrorMessage() + "\n";
    }

    return messages;
}


