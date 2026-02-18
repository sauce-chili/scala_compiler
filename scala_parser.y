%code requires {
    #include "nodes/nodes.h"
    class TopStatSeqNode;
}

%code {
    #include <iostream>
    #include <cstdio>

    // Просто объявляем функции без extern "C"
    int yylex();
    void yyerror(TopStatSeqNode** out_root, const char* s);

    using namespace std;

    const char* get_bison_token_name(int token);
}

/* для включения отладки */
%define parse.trace
%define parse.error verbose
%token-table

/* Передаем указатель на корень AST */
%parse-param { TopStatSeqNode** out_root }

%union {
    int intLiteral;
    char charLiteral;
    double doubleLiteral;
    char* stringLiteral;
    bool boolLiteral;
    char* identifier;
    char* unitLiteral;

    ClassDefNode* classDef;
    ClassParamNode* classParam;
    ClassParamsNode* classParams;
    ExtensionClassTemplateNode* extensionClassTemplate;
    DclNode* dcl;
    DefNode* def;
    ArgumentExprsNode* argumentExprs;
    AssignmentNode* assignment;
    ConstrExprNode* constrExpr;
    ExprNode* expr;
    ExprsNode* exprs;
    InfixExprNode* infixExpr;
    PrefixExprNode* prefixExpr;
    SimpleExpr1Node* simpleExpr1;
    SimpleExprNode* simpleExpr;
    FuncParamNode* funcParam;
    FuncParamsNode* funcParams;
    FunDefNode* funDef;
    FunSigNode* funSig;
    EnumeratorsNode* enumerators;
    GeneratorNode* generator;
    IdNode* id;
    ModifierNode* modifier;
    ModifiersNode* modifiers;
    BlockStatNode* blockStat;
    BlockStatsNode* blockStats;
    TopStatNode* topStat;
    TopStatSeqNode* topStatSeq;
    ClassTemplateOptNode* classTemplateOpt;
    TemplateStatNode* templateStat;
    TemplateStatsNode* templateStats;
    SimpleTypeNode* simpleType;
    SimpleTypesNode* simpleTypes;
    VarDefsNode* varDefs;
    TemplateDefNode* tmplDef;
}


%token NON

%token IF ELSE
%token FOR WHILE DO
%token LEFT_ARROW
%token YIELD
%token THROW
%token VAL VAR
%token NEW
%token RETURN
%token CLASS OBJECT DEF
%token THIS SUPER

%token PRIVATE PROTECTED OVERRIDE ABSTRACT FINAL SEALED EXTENDS
%token INT DOUBLE STRING CHAR BOOLEAN UNIT ARRAY

%token <intLiteral>DECIMAL_LITERAL
%token <charLiteral>CHAR_LITERAL
%token <doubleLiteral>DOUBLE_LITERAL
%token <stringLiteral>STRING_LITERAL
%token <boolLiteral>TRUE_LITERAL
%token <boolLiteral>FALSE_LITERAL
%token NULL_LITERAL

%token <identifier> PLUS_ASSIGNMENT MINUS_ASSIGNMENT MUL_ASSIGNMENT DIV_ASSIGNMENT MOD_ASSIGNMENT EQUAL NOT_EQUAL GREATER_OR_EQUAL LESS_OR_EQUAL
%token <identifier> ID ID_VERTICAL_SIGN ID_CIRCUMFLEX ID_AMPERSAND ID_EQUALITY ID_LESS ID_GREAT ID_COLON ID_MINUS ID_PLUS ID_ASTERISK ID_SLASH ID_PERCENT ID_EXCLAMATION ID_TILDE

%token NL

%nonassoc RETURN IF FOR NL
%nonassoc ELSE WHILE DO VAL VAR NEW YIELD MATCH CASE
%right '=' PLUS_ASSIGNMENT MINUS_ASSIGNMENT MUL_ASSIGNMENT DIV_ASSIGNMENT MOD_ASSIGNMENT
%left '|' ID_VERTICAL_SIGN
%left '^' ID_CIRCUMFLEX
%left '&' ID_AMPERSAND
%left EQUAL NOT_EQUAL ID_EQUALITY '!' ID_EXCLAMATION
%left '<' '>' GREATER_OR_EQUAL LESS_OR_EQUAL ID_LESS ID_GREAT
%right ID_COLON
%left '+' '-' ID_MINUS ID_PLUS
%left '*' '/' '%' ID_ASTERISK ID_SLASH ID_PERCENT
%left '~' ID_TILDE
%left ID '#' '?' '@' '\\'
%right UMINUS UPLUS
%right ULOGNOT UBINNOT
%left '.'
%nonassoc ':'
%nonassoc '(' '['

%type <topStatSeq> topStatSeq
%type <expr> expr
%type <assignment> assignment
%type <enumerators> enumerators
%type <generator> generator
%type <infixExpr> infixExpr
%type <prefixExpr> prefixExpr
%type <simpleExpr> simpleExpr
%type <simpleExpr1> simpleExpr1 literal
%type <argumentExprs> argumentExprs
%type <exprs> exprs
%type <blockStats> blockStats
%type <blockStat> blockStat
%type <funcParams> funcParamClause funcParams
%type <funcParam> funcParam
%type <classParams> classParamClause classParams
%type <classParam> classParam
%type <modifiers> modifiers
%type <modifier> modifier accessModifier
%type <templateStats> templateStats templateBody
%type <templateStat> templateStat
%type <dcl> dcl
%type <funSig> funSig
%type <varDefs> varDefs
%type <def> def
%type <funDef> funDef
%type <constrExpr> constrExpr
%type <classDef> classDef
%type <classTemplateOpt> classTemplateOpt
%type <extensionClassTemplate> extensionClassTemplate
%type <topStat> topStat
%type <id> fullID
%type <tmplDef> tmplDef
%type <simpleType> simpleType



%start scalaFile


%%

scalaFile: topStatSeq { *out_root = $1;}
	 ;

expr: IF '(' expr ')' nls expr ELSE expr { $$ = ExprNode::createIfElse($3, $6, $8); }
    | IF '(' expr ')' nls expr           	{ $$ = ExprNode::createIf($3, $6); }
    | WHILE '(' expr ')' nls expr              { $$ = ExprNode::createWhile($3, $6); }
    | DO expr semio WHILE '(' expr ')'         { $$ = ExprNode::createDoWhile($2, $6); }
    | RETURN                                   { $$ = ExprNode::createReturn(); }
    | RETURN expr                              { $$ = ExprNode::createReturnExpr($2); }
    | FOR '(' enumerators ')' nls expr         { $$ = ExprNode::createFor($3, $6); }
    | infixExpr                                { $$ = ExprNode::createInfix($1); }
    | assignment                               { $$ = ExprNode::createAssignment($1); }
    ;

assignment: simpleExpr '=' expr { $$ = AssignmentNode::createFieldAssignment($1, nullptr, $3); }
          ;

enumerators: generator { $$ = new EnumeratorsNode($1); }
           ;

generator: fullID ':' simpleType LEFT_ARROW expr { $$ = GeneratorNode::createGenerator($1, $3, $5); }
         ;

infixExpr: prefixExpr { $$ = InfixExprNode::createInfixFromPrefix($1); }
         | infixExpr '+' nlo infixExpr  { $$ = InfixExprNode::createFromInfixes($1, IdNode::createOperator("+"), $4); }
         | infixExpr '!' nlo infixExpr  { $$ = InfixExprNode::createFromInfixes($1, IdNode::createOperator("!"), $4); }
	 | infixExpr '#' nlo infixExpr  { $$ = InfixExprNode::createFromInfixes($1, IdNode::createOperator("#"), $4); }
         | infixExpr '%' nlo infixExpr  { $$ = InfixExprNode::createFromInfixes($1, IdNode::createOperator("%"), $4); }
	 | infixExpr '&' nlo infixExpr  { $$ = InfixExprNode::createFromInfixes($1, IdNode::createOperator("&"), $4); }
	 | infixExpr '*' nlo infixExpr  { $$ = InfixExprNode::createFromInfixes($1, IdNode::createOperator("*"), $4); }
	 | infixExpr '-' nlo infixExpr  { $$ = InfixExprNode::createFromInfixes($1, IdNode::createOperator("-"), $4); }
	 | infixExpr '/' nlo infixExpr  { $$ = InfixExprNode::createFromInfixes($1, IdNode::createOperator("/"), $4); }
	 | infixExpr '<' nlo infixExpr  { $$ = InfixExprNode::createFromInfixes($1, IdNode::createOperator("<"), $4); }
	 | infixExpr '>' nlo infixExpr  { $$ = InfixExprNode::createFromInfixes($1, IdNode::createOperator(">"), $4); }
	 | infixExpr '?' nlo infixExpr  { $$ = InfixExprNode::createFromInfixes($1, IdNode::createOperator("?"), $4); }
	 | infixExpr '@' nlo infixExpr  { $$ = InfixExprNode::createFromInfixes($1, IdNode::createOperator("@"), $4); }
	 | infixExpr '\\' nlo infixExpr { $$ = InfixExprNode::createFromInfixes($1, IdNode::createOperator("\\"), $4); }
	 | infixExpr '^' nlo infixExpr  { $$ = InfixExprNode::createFromInfixes($1, IdNode::createOperator("^"), $4); }
	 | infixExpr '~' nlo infixExpr  { $$ = InfixExprNode::createFromInfixes($1, IdNode::createOperator("~"), $4); }
	 | infixExpr PLUS_ASSIGNMENT nlo infixExpr  { $$ = InfixExprNode::createFromInfixes($1, IdNode::createOperator("+="), $4); }
	 | infixExpr MINUS_ASSIGNMENT nlo infixExpr { $$ = InfixExprNode::createFromInfixes($1, IdNode::createOperator("-="), $4); }
	 | infixExpr MUL_ASSIGNMENT nlo infixExpr   { $$ = InfixExprNode::createFromInfixes($1, IdNode::createOperator("*="), $4); }
	 | infixExpr DIV_ASSIGNMENT nlo infixExpr   { $$ = InfixExprNode::createFromInfixes($1, IdNode::createOperator("/="), $4); }
	 | infixExpr MOD_ASSIGNMENT nlo infixExpr   { $$ = InfixExprNode::createFromInfixes($1, IdNode::createOperator("%="), $4); }
	 | infixExpr EQUAL nlo infixExpr            { $$ = InfixExprNode::createFromInfixes($1, IdNode::createOperator("=="), $4); }
	 | infixExpr NOT_EQUAL nlo infixExpr        { $$ = InfixExprNode::createFromInfixes($1, IdNode::createOperator("!="), $4); }
	 | infixExpr GREATER_OR_EQUAL nlo infixExpr { $$ = InfixExprNode::createFromInfixes($1, IdNode::createOperator(">="), $4); }
	 | infixExpr LESS_OR_EQUAL nlo infixExpr    { $$ = InfixExprNode::createFromInfixes($1, IdNode::createOperator("<="), $4); }
	 | infixExpr ID nlo infixExpr               { $$ = InfixExprNode::createFromInfixes($1, IdNode::createId($2), $4); }
	 | infixExpr ID_EQUALITY nlo infixExpr      { $$ = InfixExprNode::createFromInfixes($1, IdNode::createId($2), $4); }
	 | infixExpr ID_VERTICAL_SIGN nlo infixExpr { $$ = InfixExprNode::createFromInfixes($1, IdNode::createId($2), $4); }
	 | infixExpr ID_AMPERSAND nlo infixExpr     { $$ = InfixExprNode::createFromInfixes($1, IdNode::createId($2), $4); }
	 | infixExpr ID_CIRCUMFLEX nlo infixExpr    { $$ = InfixExprNode::createFromInfixes($1, IdNode::createId($2), $4); }
	 | infixExpr ID_LESS nlo infixExpr          { $$ = InfixExprNode::createFromInfixes($1, IdNode::createId($2), $4); }
	 | infixExpr ID_GREAT nlo infixExpr         { $$ = InfixExprNode::createFromInfixes($1, IdNode::createId($2), $4); }
	 | infixExpr ID_MINUS nlo infixExpr         { $$ = InfixExprNode::createFromInfixes($1, IdNode::createId($2), $4); }
	 | infixExpr ID_PLUS nlo infixExpr          { $$ = InfixExprNode::createFromInfixes($1, IdNode::createId($2), $4); }
	 | infixExpr ID_ASTERISK nlo infixExpr      { $$ = InfixExprNode::createFromInfixes($1, IdNode::createId($2), $4); }
	 | infixExpr ID_SLASH nlo infixExpr         { $$ = InfixExprNode::createFromInfixes($1, IdNode::createId($2), $4); }
	 | infixExpr ID_PERCENT nlo infixExpr       { $$ = InfixExprNode::createFromInfixes($1, IdNode::createId($2), $4); }
	 | infixExpr ID_EXCLAMATION nlo infixExpr   { $$ = InfixExprNode::createFromInfixes($1, IdNode::createId($2), $4); }
	 | infixExpr ID_TILDE nlo infixExpr 	    { $$ = InfixExprNode::createFromInfixes($1, IdNode::createId($2), $4); }
	 | infixExpr ID_COLON nlo infixExpr         { $$ = InfixExprNode::createFromInfixes($1, IdNode::createId($2), $4); }
         ;

prefixExpr: simpleExpr { $$ = PrefixExprNode::createPrefixExprNode($1, _NO_UNARY_OPERATOR); }
          | '+' simpleExpr %prec UPLUS { $$ = PrefixExprNode::createPrefixExprNode($2, _UNARY_PLUS); }
          | '-' simpleExpr %prec UMINUS { $$ = PrefixExprNode::createPrefixExprNode($2, _UNARY_MINUS); }
          | '~' simpleExpr %prec UBINNOT { $$ = PrefixExprNode::createPrefixExprNode($2, _BIT_NOT); }
          | '!' simpleExpr %prec ULOGNOT { $$ = PrefixExprNode::createPrefixExprNode($2, _NOT); }
          ;

simpleExpr: NEW fullID argumentExprs { $$ = SimpleExprNode::createNewObjectNode($2, $3); } // (бывший constrInvoke)
	  | NEW ARRAY '[' simpleType ']' argumentExprs { $$ = SimpleExprNode::createNewArrayNode($4, $6); } // в argumentExprs должен быть 1 элемент - размер массива
          | '{' blockStats '}' { $$ = SimpleExprNode::createBlockStatsNode($2); } // бывший blockExpr
          | simpleExpr1        { $$ = SimpleExprNode::createSimpleExpr1Node($1); }
          ;

simpleExpr1: literal                   { $$ = $1; }
           | fullID                    { $$ = SimpleExpr1Node::createIdNode($1); }
           | SUPER '.' fullID          { $$ = SimpleExpr1Node::createSuperFieldAccessNode($3); }
           | THIS                      { $$ = SimpleExpr1Node::createPlainThisNode(); }
           | simpleExpr '.' fullID     { $$ = SimpleExpr1Node::createSimpleExprFieldAccessNode($3, $1); }
           | '(' expr ')'              { $$ = SimpleExpr1Node::createParenthesizedExprNode($2); }
           | simpleExpr1 argumentExprs { $$ = SimpleExpr1Node::createMethodCallNode($1, $2); } // вызов метода
           | ARRAY '[' simpleType ']' argumentExprs { $$ = SimpleExpr1Node::createArrayWithTypeBuilderNode($3, $5); }
           | ARRAY argumentExprs       {$$ = SimpleExpr1Node::createArrayBuilderNode($2); }
           ;

argumentExprs: '(' exprs ')' { $$ = new ArgumentExprsNode($2); }
             ;

exprs: /* empty */    { $$ = new ExprsNode(); }
     | expr           { $$ = ExprsNode::addExprToList(nullptr, $1); }
     | exprs ',' expr { $$ = ExprsNode::addExprToList($1, $3); }
     ;

simpleType: fullID                   { $$ = SimpleTypeNode::createIdTypeNode($1); }
          | ARRAY '[' simpleType ']' { $$ = SimpleTypeNode::createSimpleTypeNode($3); }
          ;

blockStats: blockStat                 { $$ = BlockStatsNode::addBlockStatToList(nullptr, $1); }
          | blockStats semi blockStat { $$ = BlockStatsNode::addBlockStatToList($1, $3); }
          ;

blockStat: /* empty */ { $$ = nullptr; }
	 | varDefs     { $$ = BlockStatNode::createVarDefsNode($1); }
         | expr        { $$ = BlockStatNode::createExprNode($1); }
         ;


/* --------------------- FUNC --------------------- */

funcParamClause: nlo '(' ')'            { $$ = nullptr; }
               | nlo '(' funcParams ')' { $$ = $3; }
               ;

funcParams: funcParam                { $$ = FuncParamsNode::addFuncParamToList(nullptr, $1); }
          | funcParams ',' funcParam { $$ = FuncParamsNode::addFuncParamToList($1, $3); }
          ;

funcParam: fullID ':' simpleType { $$ = FuncParamNode::createFuncParam($1, $3); }
         ;

/* --------------------- FUNC --------------------- */

/* --------------------- CLASS --------------------- */

classParamClause: nlo '(' ')'             { $$ = new ClassParamsNode(); }
                | nlo '(' classParams ')' { $$ = $3; }
                ;

classParams: classParam                 { $$ = ClassParamsNode::addClassParamToList(nullptr, $1); }
           | classParams ',' classParam { $$ = ClassParamsNode::addClassParamToList($1, $3); }
           ;

classParam: modifiers VAL fullID ':' simpleType { $$ = ClassParamNode::createClassParam(_VAL_CLASS_PARAM, $1, $3, $5); }
          | modifiers VAR fullID ':' simpleType { $$ = ClassParamNode::createClassParam(_VAR_CLASS_PARAM, $1, $3, $5); }
          ;

modifiers: /* empty */        { $$ = new ModifiersNode(); }
         | modifiers modifier { $$ = ModifiersNode::addModifierToList($1, $2); }
         ;

modifier: ABSTRACT       { $$ = ModifierNode::createModifier(_ABSTRACT); }
        | FINAL          { $$ = ModifierNode::createModifier(_FINAL); }
        | SEALED         { $$ = ModifierNode::createModifier(_SEALED); }
        | accessModifier { $$ = $1; }
        | OVERRIDE       { $$ = ModifierNode::createModifier(_OVERRIDE); }
        ;

accessModifier: PRIVATE   { $$ = ModifierNode::createModifier(_PRIVATE); }
	      | PROTECTED { $$ = ModifierNode::createModifier(_PROTECTED); }
	      ;

/* --------------------- CLASS --------------------- */

templateBody: nlo '{' templateStats '}' { $$ = TemplateStatsNode::addTemplateStatToFrontToList($3, nullptr); }
            ;

templateStats: templateStat                     { $$ = TemplateStatsNode::addTemplateStatToBackToList(nullptr, $1); }
             | templateStats semi templateStat  { $$ = TemplateStatsNode::addTemplateStatToBackToList($1, $3); }
             ;

templateStat: /* empty */   { $$ = nullptr; }
            | modifiers def { $$ = TemplateStatNode::createDefTemplate($1, $2); }
            | modifiers dcl { $$ = TemplateStatNode::createDclTemplate($1, $2); }
            ;

/* --------------------- DECL --------------------- */

dcl: VAL fullID ':' simpleType { $$ = DclNode::createValDcl($2, $4); }
   | VAR fullID ':' simpleType { $$ = DclNode::createVarDcl($2, $4); }
   | DEF funSig ':' simpleType { $$ = DclNode::createDefDcl($2, $4); }
   ;

funSig: fullID funcParamClause { $$ = FunSigNode::createFunSig($1, $2); }
      ;

/* --------------------- DECL --------------------- */

/* --------------------- DEFS --------------------- */

varDefs: VAL fullID ':' simpleType '=' expr { $$ = VarDefsNode::createVal($2, $4, $6); }
       | VAR fullID ':' simpleType '=' expr { $$ = VarDefsNode::createVar($2, $4, $6); }
       ;

def: varDefs    { $$ = DefNode::createVarDefs($1); }
   | DEF funDef { $$ = DefNode::createFunDef($2); }
   ;

funDef: funSig ':' simpleType '=' expr       { $$ = FunDefNode::createFunSigFunDef($1, $3, $5); }
      | THIS funcParamClause '=' constrExpr { $$ = FunDefNode::createThisConstrCallFunDef($2, $4); }
      ;


constrExpr: THIS argumentExprs { $$ = ConstrExprNode::createConstrExpr($2, nullptr); } // вызов первичного конструктора, бывший selfInvocation
          | '{' THIS argumentExprs semi blockStats '}' { $$ = ConstrExprNode::createConstrExpr($3, $5, true); }
          | '{' THIS argumentExprs '}' { $$ = ConstrExprNode::createConstrExpr($3, nullptr, true); }
          ;

tmplDef: CLASS classDef { $$ = TemplateDefNode::createClassDef($2); }
       ;

classDef: fullID accessModifier classParamClause classTemplateOpt { $$ = ClassDefNode::createClassDef($1, $2, $3, $4); }
        | fullID classParamClause classTemplateOpt                { $$ = ClassDefNode::createClassDef($1, nullptr, $2, $3); }
        | fullID classTemplateOpt                                 { $$ = ClassDefNode::createClassDef($1, nullptr, nullptr, $2); }
        ;

classTemplateOpt: EXTENDS extensionClassTemplate { $$ = ClassTemplateOptNode::addFuncParamToBackToList($2, nullptr); }
                | templateBody                   { $$ = ClassTemplateOptNode::addFuncParamToBackToList(nullptr, $1); }
                ;

extensionClassTemplate: fullID argumentExprs templateBody       { $$ = ExtensionClassTemplateNode::createExtendWithConstrAndBody($1, $2, $3); }
                      | fullID templateBody                     { $$ = ExtensionClassTemplateNode::createExtendWithBody($1, $2); }
                      ;

topStatSeq: topStat                 { $$ = TopStatSeqNode::addModifierToList(nullptr, $1); }
	  | topStatSeq semi topStat { $$ = TopStatSeqNode::addModifierToList($1, $3); }
          ;

topStat: modifiers tmplDef { $$ = TopStatNode::createClass($1, $2); }
       ;

/* --------------------- DEFS --------------------- */

literal: DECIMAL_LITERAL { $$ = SimpleExpr1Node::createIntNode($1); }
       | CHAR_LITERAL    { $$ = SimpleExpr1Node::createCharNode($1); }
       | DOUBLE_LITERAL  { $$ = SimpleExpr1Node::createDoubleNode($1); }
       | STRING_LITERAL  { $$ = SimpleExpr1Node::createStringNode($1); }
       | TRUE_LITERAL    { $$ = SimpleExpr1Node::createBoolNode($1); }
       | FALSE_LITERAL   { $$ = SimpleExpr1Node::createBoolNode($1); }
       | NULL_LITERAL    { $$ = SimpleExpr1Node::createNullNode(); }
       | '(' ')'         { $$ = SimpleExpr1Node::createUnitLiteralNode(); }
       ;

nls: /* empty */
   | nls NL
   ;

nlo: /* empty */
   | NL
   ;

semi: ';'
    | NL nlo // не используется nls, тк достаточно опциональности на 1 символ, тк по грамматикам Scala подряд может идит лишь 2 NL(на это есть ограничение в лексере)
    ;

semio: /* empty */
     | semi
     ;

fullID: '+'              { $$ = IdNode::createOperator("+"); }
      | '!'              { $$ = IdNode::createOperator("!"); }
      | '#'              { $$ = IdNode::createOperator("#"); }
      | '%'              { $$ = IdNode::createOperator("%"); }
      | '&'              { $$ = IdNode::createOperator("&"); }
      | '*'              { $$ = IdNode::createOperator("*"); }
      | '-'              { $$ = IdNode::createOperator("-"); }
      | '/'              { $$ = IdNode::createOperator("/"); }
      | '<'              { $$ = IdNode::createOperator("<"); }
      | '>'              { $$ = IdNode::createOperator(">"); }
      | '?'              { $$ = IdNode::createOperator("?"); }
      | '@'              { $$ = IdNode::createOperator("@"); }
      | '\\'             { $$ = IdNode::createOperator("\\"); }
      | '^'              { $$ = IdNode::createOperator("^"); }
      | '~'              { $$ = IdNode::createOperator("~"); }
      | PLUS_ASSIGNMENT  { $$ = IdNode::createOperator("+="); }
      | MINUS_ASSIGNMENT { $$ = IdNode::createOperator("-="); }
      | MUL_ASSIGNMENT   { $$ = IdNode::createOperator("*="); }
      | DIV_ASSIGNMENT   { $$ = IdNode::createOperator("/="); }
      | MOD_ASSIGNMENT   { $$ = IdNode::createOperator("%="); }
      | EQUAL            { $$ = IdNode::createOperator("=="); }
      | NOT_EQUAL        { $$ = IdNode::createOperator("!="); }
      | GREATER_OR_EQUAL { $$ = IdNode::createOperator(">="); }
      | LESS_OR_EQUAL    { $$ = IdNode::createOperator("<="); }
      | ID               { $$ = IdNode::createId($1); }
      | ID_EQUALITY      { $$ = IdNode::createId($1); }
      | ID_VERTICAL_SIGN { $$ = IdNode::createId($1); }
      | ID_AMPERSAND     { $$ = IdNode::createId($1); }
      | ID_CIRCUMFLEX    { $$ = IdNode::createId($1); }
      | ID_LESS          { $$ = IdNode::createId($1); }
      | ID_GREAT         { $$ = IdNode::createId($1); }
      | ID_MINUS         { $$ = IdNode::createId($1); }
      | ID_PLUS          { $$ = IdNode::createId($1); }
      | ID_ASTERISK      { $$ = IdNode::createId($1); }
      | ID_SLASH         { $$ = IdNode::createId($1); }
      | ID_PERCENT       { $$ = IdNode::createId($1); }
      | ID_EXCLAMATION   { $$ = IdNode::createId($1); }
      | ID_TILDE   	 { $$ = IdNode::createId($1); }
      | ID_COLON         { $$ = IdNode::createId($1); }
      ;

%%

const char* get_bison_token_name(int token) {
            return yytname[YYTRANSLATE(token)];
}