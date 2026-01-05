#include "IdsNode.h"

IdsNode::IdsNode() {
    ids = new std::list<IdNode*>;
}

IdsNode::IdsNode(IdNode *id) {
    ids = new std::list<IdNode*>;
    ids->push_back(id);
}

IdsNode *IdsNode::addIdToList(IdsNode *list, IdNode *id) {
    if (list == nullptr) {
        return new IdsNode(id);
    }

    list->ids->push_back(id);
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
