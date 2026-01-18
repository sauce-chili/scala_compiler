#include "FuncParamsNode.h"

FuncParamsNode::FuncParamsNode() {
    funcParams = new std::list<FuncParamNode*>();
}

FuncParamsNode::FuncParamsNode(FuncParamNode *first) {
    funcParams = new std::list<FuncParamNode*>();
    if (first) {
        funcParams->push_back(first);
    }
}

FuncParamsNode::FuncParamsNode(ClassParamsNode *first) {
    funcParams = new std::list<FuncParamNode*>();
    for (ClassParamNode* c: *(first->classParams)) {
        if (!c) continue;

        funcParams->push_back(FuncParamNode::createFuncParam(c->fullId->copy(), c->compoundType->copy()));
    }
}

FuncParamsNode *FuncParamsNode::addFuncParamToList(FuncParamsNode *list, FuncParamNode *funcParam) {
    if (list == nullptr) {
        return new FuncParamsNode(funcParam);
    }

    if (funcParam) {
        list->funcParams->push_back(funcParam);
    }
    return list;
}

FuncParamsNode *FuncParamsNode::copy() {
    FuncParamsNode* copied = new FuncParamsNode();

    if (funcParams) {
        copied->funcParams = new std::list<FuncParamNode*>();

        for (FuncParamNode* e: *funcParams) {
            if (e)
                copied->funcParams->push_back(e->copy());
            else
                copied->funcParams->push_back(nullptr);
        }
    }

    return copied;
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

list<Node *> FuncParamsNode::getChildren() const {
    list<Node *> children = {};
    for (Node *node : *funcParams) {
        addChildIfNotNull(children, node);
    }
    return children;
}

