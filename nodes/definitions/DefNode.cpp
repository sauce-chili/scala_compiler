#include "DefNode.h"

DefNode::DefNode() {
    varDefs = nullptr;
    funDef = nullptr;
    modifiers = nullptr;
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

DefNode * DefNode::setModifiers(ModifiersNode *modifiers) {
    this->modifiers = modifiers->copy();
    return this;
}

DefNode *DefNode::copy() {
    DefNode* node = new DefNode();
    node->type = type;

    if (varDefs) {
        node->varDefs = varDefs->copy();
    }
    if (funDef) {
        node->funDef = funDef->copy();
    }
    if (modifiers) {
        node->modifiers = modifiers->copy();
    }

    return node;
}

string DefNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, varDefs, "varDefs_");
    addDotChild(dot, funDef, "funDef_");

    return dot;
}

string DefNode::getDotLabel() const {
    return definitionTypeToString(type);
}

bool DefNode::containsVar(string name) {
    if (!varDefs) return false;

    return varDefs->ids->contains(name);
}

list<Node *> DefNode::getChildren() const {
    list<Node *> children = {};
    addChildIfNotNull(children, modifiers);
    addChildIfNotNull(children, varDefs);
    addChildIfNotNull(children, funDef);
    return children;
}
