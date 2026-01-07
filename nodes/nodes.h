//
// Created by Dmitry Smirnov on 07.01.2026.
//

#ifndef SCALA_LEXER_NODES_H
#define SCALA_LEXER_NODES_H

// Base
#include "Node.h"
#include "Types.h"

// ===== id =====
#include "id/IdNode.h"
#include "id/IdsNode.h"
#include "id/StableIdNode.h"

// ===== modifier =====
#include "modifier/ModifierNode.h"
#include "modifier/ModifiersNode.h"

// ===== type =====
#include "type/SimpleTypeNode.h"
#include "type/SimpleTypesNode.h"
#include "type/CompoundTypeNode.h"

// ===== exprs =====
#include "exprs/ExprNode.h"
#include "exprs/ExprsNode.h"
#include "exprs/ArgumentExprsNode.h"
#include "exprs/AssignExprNode.h"
#include "exprs/AssignmentNode.h"
#include "exprs/ConstrExprNode.h"
#include "exprs/InfixExprNode.h"
#include "exprs/PrefixExprNode.h"
#include "exprs/SimpleExprNode.h"
#include "exprs/SimpleExpr1Node.h"

// ===== generator =====
#include "generator/GeneratorNode.h"
#include "generator/EnumeratorPartNode.h"
#include "generator/EnumeratorsNode.h"

// ===== try =====
#include "try/TryExprNode.h"

// ===== func =====
#include "func/FuncParamNode.h"
#include "func/FuncParamsNode.h"
#include "func/FunSigNode.h"
#include "func/FunDefNode.h"

// ===== var =====
#include "var/VarDefsNode.h"

// ===== definitions =====
#include "definitions/DclNode.h"
#include "definitions/DefNode.h"

// ===== stats =====
#include "stats/BlockStatNode.h"
#include "stats/BlockStatsNode.h"
#include "stats/TopStatNode.h"
#include "stats/TopStatSeqNode.h"

// ===== class =====
#include "class/ClassDefNode.h"
#include "class/ClassParamNode.h"
#include "class/ClassParamsNode.h"
#include "class/ClassParentsNode.h"
#include "class/ClassTemplateNode.h"
#include "class/ConstrInvokeNode.h"

// ===== enum =====
#include "enum/EnumCaseNode.h"
#include "enum/EnumDefNode.h"
#include "enum/EnumStatNode.h"
#include "enum/EnumStatsNode.h"

// ===== templates =====
#include "templates/ClassTemplateOptNode.h"
#include "templates/EnumTemplateNode.h"
#include "templates/TemplateDefNode.h"
#include "templates/TemplateStatNode.h"
#include "templates/TemplateStatsNode.h"

// ===== trait =====
#include "trait/TraitTemplateNode.h"
#include "trait/TraitTemplateOptNode.h"

#endif // SCALA_LEXER_NODES_H
