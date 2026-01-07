#include "DefNode.h"

DefNode::DefNode() {

}

DefNode *DefNode::createVarDefs(VarDefsNode *varDefs) {
    DefNode* node = new DefNode();
    node->type = _VAR_DEFS;
    node->varDefs = varDefs;
    return node;
}

DefNode *DefNode::createFunDef(FunDefNode *funDef) {
    DefNode* node = new DefNode();
    node->type = _FUN_DEFINITION;
    node->funDef = funDef;
    return node;
}

DefNode *DefNode::createTmplDef(TemplateDefNode *tmplDef) {
    DefNode* node = new DefNode();
    node->type = _TEMPLATE_DEFINITION;
    node->tmplDef = tmplDef;
    return node;
}

string DefNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, varDefs, "varDefs_");
    addDotChild(dot, funDef, "funDef_");
    addDotChild(dot, tmplDef, "tmplDef_");

    return dot;
}

string DefNode::getDotLabel() const {
    return definitionTypeToString(type);
}
