#ifndef SCALA_LEXER_EXTENSIONCLASSTEMPLATENODE_H
#define SCALA_LEXER_EXTENSIONCLASSTEMPLATENODE_H

#include "../Node.h"
#include "../templates/TemplateStatsNode.h"
#include "../id/IdNode.h"
#include "../exprs/ArgumentExprsNode.h"

class ExtensionClassTemplateNode: public Node {
public:
    ClassExtensionTemplateType type;
    IdNode* fullId;
    ArgumentExprsNode* argumentExprs;
    TemplateStatsNode* templateStats; // Первым преобразованием в дереве (ClassDefNode::normalizeBody) перенеслось в classTemplateOpt для удобства анализа

    ExtensionClassTemplateNode();

    static ExtensionClassTemplateNode* createExtendWithConstrAndBody(IdNode* fullId, ArgumentExprsNode* argumentExprs, TemplateStatsNode* templateStats);
    static ExtensionClassTemplateNode* createExtendWithBody(IdNode* fullId, TemplateStatsNode* templateStats);

    ExtensionClassTemplateNode* copy();

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_EXTENSIONCLASSTEMPLATENODE_H
