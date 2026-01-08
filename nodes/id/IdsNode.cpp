#include "IdsNode.h"

IdsNode::IdsNode() {
    ids = new std::list<IdNode*>;
}

IdsNode::IdsNode(IdNode *fullId) {
    ids = new std::list<IdNode*>;
    if (fullId) {
        ids->push_back(fullId);
    }
}

IdsNode *IdsNode::addIdToList(IdsNode *list, IdNode *fullId) {
    if (list == nullptr) {
        return new IdsNode(fullId);
    }

    if (fullId) {
        list->ids->push_back(fullId);

    }
    return list;
}

string IdsNode::toDot() const {
    string dot;

    addDotNode(dot);
    if (!ids->empty()) {
        for (const auto *it : *ids) {
            addDotChild(dot, it, "id_" + to_string(it->id));
        }
    }

    return dot;
}

string IdsNode::getDotLabel() const {
    return "Ids";
}
