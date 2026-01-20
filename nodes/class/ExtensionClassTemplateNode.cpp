#include "ExtensionClassTemplateNode.h"

ExtensionClassTemplateNode::ExtensionClassTemplateNode() {
    fullId = nullptr;
    argumentExprs = nullptr;
    templateStats = nullptr;
}

ExtensionClassTemplateNode *
ExtensionClassTemplateNode::createExtendWithConstrAndBody(IdNode *fullId, ArgumentExprsNode *argumentExprs,
                                                          TemplateStatsNode *templateStats) {
    ExtensionClassTemplateNode* node = new ExtensionClassTemplateNode();
    node->type = _PARENT_CONSTR_CALL_AND_THIS_BODY;
    node->fullId = fullId;
    node->argumentExprs = argumentExprs;
    node->templateStats = templateStats;
    return node;
}

ExtensionClassTemplateNode *
ExtensionClassTemplateNode::createExtendWithBody(IdNode *fullId, TemplateStatsNode *templateStats) {
    ExtensionClassTemplateNode* node = new ExtensionClassTemplateNode();
    node->type = _THIS_BODY;
    node->fullId = fullId;
    node->templateStats = templateStats;
    return node;
}

ExtensionClassTemplateNode *
ExtensionClassTemplateNode::createExtendWithConstr(IdNode *fullId, ArgumentExprsNode *argumentExprs) {
    ExtensionClassTemplateNode* node = new ExtensionClassTemplateNode();
    node->type = _PARENT_CONSTR_CALL;
    node->fullId = fullId;
    node->argumentExprs = argumentExprs;
    return node;
}

ExtensionClassTemplateNode *ExtensionClassTemplateNode::createEmptyExtend(IdNode *fullId) {
    ExtensionClassTemplateNode* node = new ExtensionClassTemplateNode();
    node->type = _ONLY_PARENT_ID;
    node->fullId = fullId;
    return node;
}

ExtensionClassTemplateNode *ExtensionClassTemplateNode::copy() {
    ExtensionClassTemplateNode* copied = new ExtensionClassTemplateNode();

    copied->type = type;
    if (fullId) {
        copied->fullId = fullId->copy();
    }
    if (templateStats) {
        copied->templateStats = templateStats->copy();
    }
    if (argumentExprs) {
        copied->argumentExprs = argumentExprs->copy();
    }

    return copied;
}

string ExtensionClassTemplateNode::toDot() const {
    string dot;

    addDotNode(dot);
    addDotChild(dot, fullId, "id_");
    addDotChild(dot, argumentExprs, "argumentExprs_");
    addDotChild(dot, templateStats, "templateStats_");

    return dot;
}

string ExtensionClassTemplateNode::getDotLabel() const {
    return "Class template " + classExtensionTemplateTypeToString(type);
}

