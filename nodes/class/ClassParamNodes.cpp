#include "ClassParamNodes.h"

ClassParamNodes::ClassParamNodes(ClassParamNode *first) {
    classParams = new std::list<ClassParamNode*>();
    classParams->push_back(first);
}

ClassParamNodes::ClassParamNodes() {
    classParams = new std::list<ClassParamNode*>();
}

ClassParamNodes *ClassParamNodes::addExprToList(ClassParamNodes *list, ClassParamNode *classParam) {
    if (list == nullptr) {
        return new ClassParamNodes(classParam);
    }

    list->classParams->push_back(classParam);
    return list;
}

string ClassParamNodes::toDot() const {
    string dot;

    addDotNode(dot);
    if (!classParams->empty()) {
        int i = 0;
        for (const auto *it : *classParams) {
            addDotChild(dot, it, "classParam_" + to_string(i));
            i++;
        }
    }

    return dot;
}

string ClassParamNodes::getDotLabel() const {
    return "Class params";
}
