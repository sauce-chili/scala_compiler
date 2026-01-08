#include "EnumTemplateNode.h"

EnumTemplateNode::EnumTemplateNode() {
    classParents = nullptr;
    enumStats = nullptr;
}

EnumTemplateNode *EnumTemplateNode::createWithClassParents(ClassParentsNode *classParents, EnumStatsNode *enumStats) {
    EnumTemplateNode* node = new EnumTemplateNode();
    node->classParents = classParents;
    node->enumStats = enumStats;
    return node;
}

EnumTemplateNode *EnumTemplateNode::createWithEnumBody(EnumStatsNode *enumStats) {
    EnumTemplateNode* node = new EnumTemplateNode();
    node->enumStats = enumStats;
    return node;
}

string EnumTemplateNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, classParents, "classParents_");
    addDotChild(dot, enumStats, "enumStats_");

    return dot;
}

string EnumTemplateNode::getDotLabel() const {
    return "Enum template";
}
