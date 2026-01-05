#include "FuncParamsNode.h"

FuncParamsNode::FuncParamsNode() {
    funcParams = new std::list<FuncParamNode*>();
}

FuncParamsNode::FuncParamsNode(FuncParamNode *first) {
    funcParams = new std::list<FuncParamNode*>();
    funcParams->push_back(first);
}

FuncParamsNode *FuncParamsNode::addFuncParamToList(FuncParamsNode *list, FuncParamNode *funcParam) {
    if (list == nullptr) {
        return new FuncParamsNode(funcParam);
    }

    list->funcParams->push_back(funcParam);
    return list;
}

string FuncParamsNode::toDot() const {
    string dot;

    addDotNode(dot);
    if (!funcParams->empty()) {
        int i = 0;
        for (const auto *it : *funcParams) {
            addDotChild(dot, it, "funcParam_" + to_string(i));
            i++;
        }
    }

    return dot;
}

string FuncParamsNode::getDotLabel() const {
    return "Func params";
}
