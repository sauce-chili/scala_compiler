#include <iostream>
#include <fstream>
#include <sstream>

#include "nodes/nodes.h"

class TreeToCodeConverter {
private:
    std::ostringstream out;
    int indentLevel = 0;
    const std::string indentStr = "  ";

    std::string indent() const {
        std::string result;
        for (int i = 0; i < indentLevel; ++i) {
            result += indentStr;
        }
        return result;
    }

public:
    std::string generate(TopStatSeqNode* root) {
        if (!root || !root->topStats) return "";

        for (auto* topStat : *root->topStats) {
            generateTopStat(topStat);
        }
        return out.str();
    }

private:
    void generateTopStat(TopStatNode* node) {
        if (!node) return;

        if (node->tmplDef) {
            generateTemplateDef(node->tmplDef);
        }
    }

    void generateTemplateDef(TemplateDefNode* node) {
        if (!node) return;

        if (node->classDef) {
            generateClassDef(node->classDef);
        }
    }

    void generateClassDef(ClassDefNode* node) {
        if (!node) return;

        // Модификаторы класса
        if (node->modifiers) {
            generateModifiers(node->modifiers);
        }

        out << "class ";

        // Имя класса
        if (node->fullId) {
            out << node->fullId->name;
        }

        // Параметры первичного конструктора
        if (node->classParams) {
            generateClassParams(node->classParams);
        }

        // Extends
        if (node->classTemplateOpt && node->classTemplateOpt->extensionPartClassTemplate) {
            out << " extends ";
            generateExtensionClassTemplate(node->classTemplateOpt->extensionPartClassTemplate);
        }

        // Тело класса
        if (node->classTemplateOpt && node->classTemplateOpt->templateStats) {
            out << " {\n";
            indentLevel++;
            generateTemplateStats(node->classTemplateOpt->templateStats);
            indentLevel--;
            out << indent() << "}\n";
        } else {
            out << "\n";
        }
    }

    void generateModifiers(ModifiersNode* node) {
        if (!node || !node->modifiers) return;

        for (auto* mod : *node->modifiers) {
            if (mod) {
                switch (mod->type) {
                    case _PRIVATE: out << "private "; break;
                    case _PROTECTED: out << "protected "; break;
                    case _ABSTRACT: out << "abstract "; break;
                    case _FINAL: out << "final "; break;
                    case _SEALED: out << "sealed "; break;
                    case _OVERRIDE: out << "override "; break;
                    default: break;
                }
            }
        }
    }

    void generateClassParams(ClassParamsNode* node) {
        if (!node) return;

        out << "(";
        if (node->classParams) {
            bool first = true;
            for (auto* param : *node->classParams) {
                if (!first) out << ", ";
                first = false;
                generateClassParam(param);
            }
        }
        out << ")";
    }

    void generateClassParam(ClassParamNode* param) {
        if (!param) return;

        if (param->modifiers) {
            generateModifiers(param->modifiers);
        }

        if (param->type == _VAL_CLASS_PARAM) {
            out << "val ";
        } else if (param->type == _VAR_CLASS_PARAM) {
            out << "var ";
        }

        if (param->fullId) {
            out << param->fullId->name;
        }

        if (param->simpleType) {
            out << ": ";
            generateSimpleType(param->simpleType);
        }
    }

    void generateExtensionClassTemplate(ExtensionClassTemplateNode* node) {
        if (!node) return;

        if (node->fullId) {
            out << node->fullId->name;
        }

        if (node->argumentExprs) {
            generateArgumentExprs(node->argumentExprs);
        }
    }

    void generateTemplateStats(TemplateStatsNode* node) {
        if (!node || !node->templateStats) return;

        for (auto* stat : *node->templateStats) {
            generateTemplateStat(stat);
        }
    }

    void generateTemplateStat(TemplateStatNode* node) {
        if (!node) return;

        out << indent();

        if (node->def) {
            generateDef(node->def);
        } else if (node->dcl) {
            generateDcl(node->dcl);
        }
    }

    void generateDef(DefNode* node) {
        if (!node) return;

        if (node->modifiers) {
            generateModifiers(node->modifiers);
        }

        if (node->varDefs) {
            generateVarDefs(node->varDefs);
        } else if (node->funDef) {
            generateFunDef(node->funDef);
        }
    }

    void generateDcl(DclNode* node) {
        if (!node) return;

        if (node->modifiers) {
            generateModifiers(node->modifiers);
        }

        if (node->type == _VAL_DECL) {
            out << "val ";
        } else if (node->type == _VAR_DECL) {
            out << "var ";
        } else if (node->type == _FUN_DEF) {
            out << "def ";
            if (node->funSig) {
                generateFunSig(node->funSig);
            }
        }

        if (node->fullId) {
            out << node->fullId->name;
        }

        if (node->simpleType) {
            out << ": ";
            generateSimpleType(node->simpleType);
        }

        out << "\n";
    }

    void generateVarDefs(VarDefsNode* node) {
        if (!node) return;

        if (node->type == _VAL_DECL) {
            out << "val ";
        } else {
            out << "var ";
        }

        if (node->fullId) {
            out << node->fullId->name;
        }

        if (node->simpleType) {
            out << ": ";
            generateSimpleType(node->simpleType);
        }

        if (node->expr) {
            out << " = ";
            generateExpr(node->expr);
        }

        out << "\n";
    }

    void generateFunDef(FunDefNode* node) {
        if (!node) return;

        out << "def ";

        bool isConstructor = false;
        if (node->funSig) {
            generateFunSig(node->funSig);
            // Проверяем, является ли это конструктором (def this(...))
            if (node->funSig->fullId && node->funSig->fullId->name == "this") {
                isConstructor = true;
            }
        } else if (node->funcParams) {
            // Вторичный конструктор
            out << "this";
            generateFuncParams(node->funcParams);
            isConstructor = true;
        }

        // Конструкторы не имеют явного типа возврата в Scala
        if (node->simpleType && !isConstructor) {
            out << ": ";
            generateSimpleType(node->simpleType);
        }

        if (node->expr) {
            out << " = ";
            generateExpr(node->expr);
        } else if (node->constrExpr) {
            out << " = {\n";
            indentLevel++;
            generateConstrExpr(node->constrExpr);
            indentLevel--;
            out << indent() << "}";
        }

        out << "\n";
    }

    void generateFunSig(FunSigNode* node) {
        if (!node) return;

        if (node->fullId) {
            out << node->fullId->name;
        }

        if (node->params) {
            generateFuncParams(node->params);
        }
    }

    void generateFuncParams(FuncParamsNode* node) {
        if (!node) return;

        out << "(";
        if (node->funcParams) {
            bool first = true;
            for (auto* param : *node->funcParams) {
                if (!first) out << ", ";
                first = false;
                generateFuncParam(param);
            }
        }
        out << ")";
    }

    void generateFuncParam(FuncParamNode* param) {
        if (!param) return;

        if (param->fullId) {
            out << param->fullId->name;
        }

        if (param->simpleType) {
            out << ": ";
            generateSimpleType(param->simpleType);
        }
    }

    void generateConstrExpr(ConstrExprNode* node) {
        if (!node) return;

        out << indent() << "this";
        if (node->argumentExprs) {
            generateArgumentExprs(node->argumentExprs);
        }
        out << "\n";

        if (node->blockStats) {
            generateBlockStats(node->blockStats);
        }
    }

    void generateSimpleType(SimpleTypeNode* node) {
        if (!node) return;

        if (node->fullId) {
            out << node->fullId->name;
        }

        if (node->arrayElemType) {
            out << "[";
            generateSimpleType(node->arrayElemType);
            out << "]";
        }
    }

    void generateExpr(ExprNode* node) {
        if (!node) return;

        switch (node->type) {
            case _IF:
                out << "if (";
                if (node->exprs && node->exprs->size() >= 2) {
                    auto it = node->exprs->begin();
                    generateExpr(*it++);
                    out << ") ";
                    generateExpr(*it);
                }
                break;

            case _IF_ELSE:
                out << "if (";
                if (node->exprs && node->exprs->size() >= 3) {
                    auto it = node->exprs->begin();
                    generateExpr(*it++);
                    out << ") ";
                    generateExpr(*it++);
                    out << " else ";
                    generateExpr(*it);
                }
                break;

            case _WHILE:
                out << "while (";
                if (node->exprs && node->exprs->size() >= 2) {
                    auto it = node->exprs->begin();
                    generateExpr(*it++);
                    out << ") ";
                    generateExpr(*it);
                }
                break;

            case _DO_WHILE:
                out << "do ";
                if (node->exprs && node->exprs->size() >= 2) {
                    auto it = node->exprs->begin();
                    generateExpr(*it++);
                    out << " while (";
                    generateExpr(*it);
                    out << ")";
                }
                break;

            case _FOR_WITHOUT_YIELD:
                out << "for (";
                if (node->enumerators) {
                    generateEnumerators(node->enumerators);
                }
                out << ") ";
                if (node->exprs && !node->exprs->empty()) {
                    generateExpr(node->exprs->front());
                }
                break;

            case _RETURN_EMPTY:
                out << "return";
                break;

            case _RETURN_EXPR:
                out << "return ";
                if (node->exprs && !node->exprs->empty()) {
                    generateExpr(node->exprs->front());
                }
                break;

            case _INFIX:
                if (node->infixExpr) {
                    generateInfixExpr(node->infixExpr);
                }
                break;

            case _ASSIGNMENT:
                if (node->assignment) {
                    generateAssignment(node->assignment);
                }
                break;

            default:
                break;
        }
    }

    void generateEnumerators(EnumeratorsNode* node) {
        if (!node || !node->generator) return;

        generateGenerator(node->generator);
    }

    void generateGenerator(GeneratorNode* node) {
        if (!node) return;

        if (node->fullId) {
            out << node->fullId->name;
        }

        if (node->simpleType) {
            out << ": ";
            generateSimpleType(node->simpleType);
        }

        out << " <- ";

        if (node->expr) {
            generateExpr(node->expr);
        }
    }

    void generateInfixExpr(InfixExprNode* node) {
        if (!node) return;

        if (node->prefixExpr) {
            generatePrefixExpr(node->prefixExpr);
        }

        if (node->right && node->fullId) {
            out << " " << node->fullId->name << " ";
            generateInfixExpr(node->right);
        }
    }

    void generatePrefixExpr(PrefixExprNode* node) {
        if (!node) return;

        switch (node->type) {
            case _UNARY_PLUS: out << "+"; break;
            case _UNARY_MINUS: out << "-"; break;
            case _NOT: out << "!"; break;
            case _BIT_NOT: out << "~"; break;
            default: break;
        }

        if (node->simpleExpr) {
            generateSimpleExpr(node->simpleExpr);
        }
    }

    void generateSimpleExpr(SimpleExprNode* node) {
        if (!node) return;

        switch (node->type) {
            case _INSTANCE_CREATING:
                out << "new " << node->fullId->name;
                if (node->arguments) {
                    generateArgumentExprs(node->arguments);
                }
                break;

            case _ARRAY_CREATING:
                out << "new ";
                if (node->simpleType) {
                    generateSimpleType(node->simpleType);
                }
                if (node->arguments) {
                    generateArgumentExprs(node->arguments);
                }
                break;

            case _BLOCK_STATS:
                out << "{\n";
                indentLevel++;
                if (node->blockStats) {
                    generateBlockStats(node->blockStats);
                }
                indentLevel--;
                out << indent() << "}";
                break;

            case _SIMPLE_EXPR_1:
                if (node->simpleExpr1) {
                    generateSimpleExpr1(node->simpleExpr1);
                }
                break;

            default:
                break;
        }
    }

    void generateSimpleExpr1(SimpleExpr1Node* node) {
        if (!node) return;

        switch (node->type) {
            case _INTEGER_LITERAL:
                out << node->intValue;
                break;
            case _DOUBLE_LITERAL:
                out << node->doubleValue;
                break;
            case _CHAR_LITERAL:
                out << "'" << node->charValue << "'";
                break;
            case _STRING_LITERAL:
                out << "\"" << node->stringValue << "\"";
                break;
            case _BOOL_LITERAL:
                out << (node->boolValue ? "true" : "false");
                break;
            case _NULL_LITERAL:
                out << "null";
                break;
            case _IDENTIFIER:
                if (node->identifier) {
                    out << node->identifier->name;
                }
                break;
            case _PLAIN_THIS:
                out << "this";
                break;
            case _SUPER_FIELD_ACCESS:
                out << "super";
                if (node->identifier) {
                    out << "." << node->identifier->name;
                }
                break;
            case _THIS_FIELD_ACCESS:
                out << "this";
                if (node->identifier) {
                    out << "." << node->identifier->name;
                }
                break;
            case _EXPRESSION_FIELD_ACCESS:
                if (node->simpleExpr) {
                    generateSimpleExpr(node->simpleExpr);
                }
                if (node->identifier) {
                    out << "." << node->identifier->name;
                }
                break;
            case _ARGUMENT_CALL:
                out << "(";
                if (node->expr) {
                    generateExpr(node->expr);
                }
                out << ")";
                break;
            case _EMPTY_CALL:
                out << "()";
                break;
            case _METHOD_CALL:
                if (node->simpleExpr1) {
                    generateSimpleExpr1(node->simpleExpr1);
                }
                if (node->argumentExprs) {
                    generateArgumentExprs(node->argumentExprs);
                }
                break;
            case _ARRAY_BUILDER:
                out << "Array";
                if (node->argumentExprs) {
                    generateArgumentExprs(node->argumentExprs);
                }
                break;
            case _ARRAY_WITH_TYPE_BUILDER:
                out << "Array[";
                if (node->simpleType) {
                    generateSimpleType(node->simpleType);
                }
                out << "]";
                if (node->argumentExprs) {
                    generateArgumentExprs(node->argumentExprs);
                }
                break;
            default:
                break;
        }
    }

    void generateArgumentExprs(ArgumentExprsNode* node) {
        if (!node) return;

        out << "(";
        if (node->exprs && node->exprs->exprs) {
            bool first = true;
            for (auto* expr : *node->exprs->exprs) {
                if (!first) out << ", ";
                first = false;
                generateExpr(expr);
            }
        }
        out << ")";
    }

    void generateAssignment(AssignmentNode* node) {
        if (!node) return;

        if (node->simpleExpr) {
            generateSimpleExpr(node->simpleExpr);
        }

        out << " = ";

        if (node->expr) {
            generateExpr(node->expr);
        }
    }

    void generateBlockStats(BlockStatsNode* node) {
        if (!node || !node->blockStats) return;

        for (auto* stat : *node->blockStats) {
            generateBlockStat(stat);
        }
    }

    void generateBlockStat(BlockStatNode* node) {
        if (!node) return;

        out << indent();

        if (node->varDefs) {
            generateVarDefs(node->varDefs);
        } else if (node->expr) {
            generateExpr(node->expr);
            out << "\n";
        }
    }
};

inline void createScalaCode(TopStatSeqNode* root, const std::string& filename) {
    if (root == nullptr) {
        std::cerr << "Error: root == nullptr\n";
        return;
    }

    std::ofstream out(filename);
    if (!out) {
        std::cerr << "Cannot open file: " << filename << "\n";
        return;
    }

    TreeToCodeConverter generator;
    out << generator.generate(root);
    out.close();

    std::cout << "Scala code saved to " << filename << "\n";
}
