#include "ClassDefNode.h"

ClassDefNode::ClassDefNode() {

}

ClassDefNode *ClassDefNode::createClassDef(IdNode *fullId, ModifierNode *modifier, ClassParamsNode *classParams,
                                           ClassTemplateOptNode *classTemplateOpt) {
    ClassDefNode* node = new ClassDefNode();
    node->fullId = fullId;
    node->modifier = modifier;
    node->classParams = classParams;
    node->classTemplateOpt = classTemplateOpt;
    return node;
}

string ClassDefNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, fullId, "id_");
    addDotChild(dot, modifier, "modifier_");
    addDotChild(dot, classParams, "classParams_");
    addDotChild(dot, classTemplateOpt, "classTemplateOpt_");

    return dot;
}

string ClassDefNode::getDotLabel() const {
    return "Class definition";
}
