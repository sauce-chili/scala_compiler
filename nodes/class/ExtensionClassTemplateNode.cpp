#include "ExtensionClassTemplateNode.h"

ExtensionClassTemplateNode::ExtensionClassTemplateNode() {
    idNode = nullptr;
    argumentExprs = nullptr;
    templateStats = nullptr;
}

ExtensionClassTemplateNode *
ExtensionClassTemplateNode::createExtendWithConstrAndBody(IdNode *idNode, ArgumentExprsNode *argumentExprs,
                                                          TemplateStatsNode *templateStats) {
    ExtensionClassTemplateNode* node = new ExtensionClassTemplateNode();
    node->type = _PARENT_CONSTR_CALL_AND_THIS_BODY;
    node->idNode = idNode;
    node->argumentExprs = argumentExprs;
    node->templateStats = templateStats;
    return node;
}

ExtensionClassTemplateNode *
ExtensionClassTemplateNode::createExtendWithBody(IdNode *idNode, TemplateStatsNode *templateStats) {
    ExtensionClassTemplateNode* node = new ExtensionClassTemplateNode();
    node->type = _THIS_BODY;
    node->idNode = idNode;
    node->templateStats = templateStats;
    return node;
}

ExtensionClassTemplateNode *
ExtensionClassTemplateNode::createExtendWithConstr(IdNode *idNode, ArgumentExprsNode *argumentExprs) {
    ExtensionClassTemplateNode* node = new ExtensionClassTemplateNode();
    node->type = _PARENT_CONSTR_CALL;
    node->idNode = idNode;
    node->argumentExprs = argumentExprs;
    return node;
}

ExtensionClassTemplateNode *ExtensionClassTemplateNode::createEmptyExtend(IdNode *idNode) {
    ExtensionClassTemplateNode* node = new ExtensionClassTemplateNode();
    node->type = _ONLY_PARENT_ID;
    node->idNode = idNode;
    return node;
}

string ExtensionClassTemplateNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, idNode, "id_");
    addDotChild(dot, argumentExprs, "argumentExprs_");
    addDotChild(dot, templateStats, "templateStats_");

    return dot;
}

string ExtensionClassTemplateNode::getDotLabel() const {
    return "Class template " + classExtensionTemplateTypeToString(type);
}
