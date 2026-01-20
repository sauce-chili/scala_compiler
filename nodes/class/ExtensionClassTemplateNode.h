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
    TemplateStatsNode* templateStats;

    ExtensionClassTemplateNode();

    static ExtensionClassTemplateNode* createExtendWithConstrAndBody(IdNode* fullId, ArgumentExprsNode* argumentExprs, TemplateStatsNode* templateStats);
    static ExtensionClassTemplateNode* createExtendWithBody(IdNode* fullId, TemplateStatsNode* templateStats);
    static ExtensionClassTemplateNode* createExtendWithConstr(IdNode* fullId, ArgumentExprsNode* argumentExprs);
    static ExtensionClassTemplateNode* createEmptyExtend(IdNode* fullId);

    ExtensionClassTemplateNode* copy();

    string toDot() const override;
    string getDotLabel() const override;
};


#endif //SCALA_LEXER_EXTENSIONCLASSTEMPLATENODE_H
