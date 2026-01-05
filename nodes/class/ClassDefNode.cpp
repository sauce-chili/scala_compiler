#include "ClassDefNode.h"

ClassDefNode::ClassDefNode() {

}

ClassDefNode *ClassDefNode::createClassDef(IdNode *id, ModifierNode *modifier, ClassParamNodes *classParams,
                                           ClassTemplateOptNode *classTemplateOpt) {
    ClassDefNode* node = new ClassDefNode();
    node->id = id;
    node->modifier = modifier;
    node->classParams = classParams;
    node->classTemplateOpt = classTemplateOpt;
    return node;
}

string ClassDefNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, id, "id_");
    addDotChild(dot, modifier, "modifier_");
    addDotChild(dot, classParams, "classParams_");
    addDotChild(dot, classTemplateOpt, "classTemplateOpt_");

    return dot;
}

string ClassDefNode::getDotLabel() const {
    return "Class definition";
}
