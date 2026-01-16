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
    ClassParentsNode* classParents;
    DclNode* dcl;
    DefNode* def;
    EnumCaseNode* enumCase;
    EnumDefNode* enumDef;
    EnumStatNode* enumStat;
    EnumStatsNode* enumStats;
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
    EnumeratorPartNode* enumeratorPart;
    EnumeratorsNode* enumerators;
    GeneratorNode* generator;
    IdNode* id;
    IdsNode* ids;
    StableIdNode* stableId;
    ModifierNode* modifier;
    ModifiersNode* modifiers;
    BlockStatNode* blockStat;
    BlockStatsNode* blockStats;
    TopStatNode* topStat;
    TopStatSeqNode* topStatSeq;
    ClassTemplateOptNode* classTemplateOpt;
    EnumTemplateNode* enumTemplate;
    TemplateDefNode* tmplDef;
    TemplateStatNode* templateStat;
    TemplateStatsNode* templateStats;
    TraitTemplateNode* traitTemplate;
    TraitTemplateOptNode* traitTemplateOpt;
    TryExprNode* tryExpr;
    CompoundTypeNode* compoundType;
    SimpleTypeNode* simpleType;
    SimpleTypesNode* simpleTypes;
    VarDefsNode* varDefs;
}


%token NON

%token IF ELSE
%token FOR WHILE DO
%token TRY CATCH FINALLY LEFT_ARROW
%token YIELD
%token THROW
%token VAL VAR
%token NEW
%token RETURN
%token CLASS OBJECT DEF TRAIT ENUM
%token WITH
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

%nonassoc LOW_PREC
%nonassoc RETURN IF FOR NL
%nonassoc ELSE WHILE DO TRY THROW VAL VAR NEW YIELD MATCH CASE
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
%nonassoc CATCH
%nonassoc FINALLY
%nonassoc END_TEMPLATE

%type <topStatSeq> topStatSeq
%type <expr> expr
%type <assignment> assignment
%type <enumerators> enumerators
%type <enumeratorPart> enumeratorPart
%type <generator> generator
%type <compoundType> compoundTypeO compoundType
%type <infixExpr> infixExpr
%type <prefixExpr> prefixExpr
%type <simpleExpr> simpleExpr
%type <simpleExpr1> simpleExpr1 literal
%type <argumentExprs> argumentExprs
%type <exprs> exprs
%type <simpleType> simpleType
%type <stableId> stableId
%type <blockStats> blockStats
%type <blockStat> blockStat
%type <ids> ids
%type <tryExpr> tryExpr
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
%type <tmplDef> tmplDef
%type <classDef> classDef
%type <enumDef> enumDef
%type <classTemplateOpt> classTemplateOpt
%type <traitTemplateOpt> traitTemplateOpt
%type <enumTemplate> enumTemplate
%type <classParents> classParents
%type <traitTemplate> traitTemplate
%type <enumStats> enumStats enumBody
%type <enumStat> enumStat
%type <enumCase> enumCase
%type <topStat> topStat
%type <simpleTypes> simpleTypes
%type <id> fullID


%start scalaFile


%%

scalaFile: topStatSeq { *out_root = $1;}
	 ;

expr: IF '(' expr ')' nls expr ELSE expr { $$ = ExprNode::createIfElse($3, $6, $8); }
    | IF '(' expr ')' nls expr           	{ $$ = ExprNode::createIf($3, $6); }
    | WHILE '(' expr ')' nls expr              { $$ = ExprNode::createWhile($3, $6); }
    | tryExpr                                  { $$ = ExprNode::createTry($1); }
    | DO expr semio WHILE '(' expr ')'         { $$ = ExprNode::createDoWhile($2, $6); }
    | THROW expr                               { $$ = ExprNode::createThrow($2); }
    | RETURN                                   { $$ = ExprNode::createReturn(); }
    | RETURN expr                              { $$ = ExprNode::createReturnExpr($2); }
    | FOR '(' enumerators ')' nls expr         { $$ = ExprNode::createFor($3, $6); }
    | FOR '(' enumerators ')' nls YIELD expr   { $$ = ExprNode::createForYield($3, $7); }
    | FOR '{' enumerators '}' nls expr         { $$ = ExprNode::createFor($3, $6); }
    | FOR '{' enumerators '}' nls YIELD expr   { $$ = ExprNode::createForYield($3, $7); }
    | infixExpr                                { $$ = ExprNode::createInfix($1); }
    | assignment                               { $$ = ExprNode::createAssignment($1); }
    ;

assignment: fullID '=' expr                    { $$ = AssignmentNode::createIdAssignment($1, $3); }
          | simpleExpr '.' fullID '=' expr     { $$ = AssignmentNode::createFieldAssignment($1, $3, $5); }
          | simpleExpr1 argumentExprs '=' expr { $$ = AssignmentNode::createArrayAssignment($1, $2, $4); } // запись в массив
          ;

enumerators: generator                       { $$ = new EnumeratorsNode($1); }
           | enumerators semi enumeratorPart { $$ = EnumeratorsNode::addModifierToList($1, $3); }
           ;

enumeratorPart: generator                     { $$ = EnumeratorPartNode::createGeneratorEnumeratorPart($1); }
              | fullID compoundTypeO '=' expr { $$ = EnumeratorPartNode::createVarDefEnumeratorPart($1, $2, $4); } // определение переменной
              ;

generator: fullID compoundTypeO LEFT_ARROW expr { $$ = GeneratorNode::createGenerator($1, $2, $4); }
         ;

compoundTypeO: /* empty */                { $$ = nullptr; }
          | ':' compoundType              { $$ = $2; }
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
	 | infixExpr EQUAL nlo infixExpr            { $$ = InfixExprNode::createFromInfixes($1, IdNode::createOperator("="), $4); }
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

simpleExpr: NEW stableId argumentExprs { $$ = SimpleExprNode::createConstrInvokeNode(ConstrInvokeNode::createConstrInvokeNode($2, $3)); } // (бывший constrInvoke)
	  | NEW stableId { $$ = SimpleExprNode::createConstrInvokeNode(ConstrInvokeNode::createConstrInvokeNode($2, nullptr)); } // (бывший constrInvoke)
	  | NEW ARRAY '[' compoundType ']' argumentExprs { $$ = SimpleExprNode::createArrayCreatingNode(SimpleExpr1Node::createArrayWithTypeBuilderNode($4, $6)); }
	  | NEW ARRAY argumentExprs { $$ = SimpleExprNode::createArrayCreatingNode(SimpleExpr1Node::createArrayBuilderNode($3)); } // argumentExprs принимает только 1 аргумент (размер массива)
          | '{' blockStats '}' { $$ = SimpleExprNode::createBlockStatsNode($2); } // бывший blockExpr
          | simpleExpr1        { $$ = SimpleExprNode::createSimpleExpr1Node($1); }
          ;

simpleExpr1: literal                   { $$ = $1; }
           | fullID                    { $$ = SimpleExpr1Node::createIdNode($1); }
           | SUPER '.' fullID          { $$ = SimpleExpr1Node::createSuperFieldAccessNode($3); }
           | THIS                      { $$ = SimpleExpr1Node::createPlainThisNode(); }
           | simpleExpr '.' THIS       { $$ = SimpleExpr1Node::createSimpleExprFieldAccessNode(nullptr, $1); }
           | simpleExpr '.' fullID     { $$ = SimpleExpr1Node::createSimpleExprFieldAccessNode($3, $1); }
           | '(' expr ')'              { $$ = SimpleExpr1Node::createArgumentCallNode($2); }
           | '(' ')'                   { $$ = SimpleExpr1Node::createEmptyCallNode(); }
           | simpleExpr1 argumentExprs { $$ = SimpleExpr1Node::createMethodCallNode($1, $2); } // вызов метода
           | ARRAY '[' compoundType ']' argumentExprs { $$ = SimpleExpr1Node::createArrayWithTypeBuilderNode($3, $5); }
           | ARRAY argumentExprs       {$$ = SimpleExpr1Node::createArrayBuilderNode($2); }
           ;

argumentExprs: '(' exprs ')' { $$ = new ArgumentExprsNode($2); }
             ;

exprs: /* empty */    { $$ = new ExprsNode(); }
     | expr           { $$ = ExprsNode::addExprToList(nullptr, $1); }
     | exprs ',' expr { $$ = ExprsNode::addExprToList($1, $3); }
     ;

compoundType: simpleType                   { $$ = CompoundTypeNode::addStableId(nullptr, $1); }
            | compoundType WITH simpleType { $$ = CompoundTypeNode::addStableId($1, $3); }
            ;

simpleType: stableId                   { $$ = SimpleTypeNode::createStableIdNode($1); }
          | ARRAY '[' compoundType ']' { $$ = SimpleTypeNode::createArrayWithCompoundTypeNode($3); }
          ;

stableId: fullID              { $$ = StableIdNode::createStableIdByFullId($1); }
        | SUPER '.' fullID    { $$ = StableIdNode::createSuperCallStableId($3); }
        | THIS '.' fullID     { $$ = StableIdNode::createThisCallStableIdBy($3); }
        | stableId '.' fullID { $$ = StableIdNode::createRecursiveStableId($1, $3); }
        ;

blockStats: blockStat                 { $$ = BlockStatsNode::addBlockStatToList(nullptr, $1); }
          | blockStats semi blockStat { $$ = BlockStatsNode::addBlockStatToList($1, $3); }
          ;

blockStat: /* empty */ { $$ = nullptr; }
	 | varDefs     { $$ = BlockStatNode::createVarDefsNode($1); }
         | expr        { $$ = BlockStatNode::createExprNode($1); }
         ;

ids: fullID         { $$ = IdsNode::addIdToList(nullptr, $1); }
   | ids ',' fullID { $$ = IdsNode::addIdToList($1, $3); }
   ;

/* --------------------- TRY --------------------- */

tryExpr: TRY expr                         { $$ = TryExprNode::createExceptionBlock($2, nullptr, nullptr); }
       | TRY expr CATCH expr              { $$ = TryExprNode::createExceptionBlock($2, $4, nullptr); }
       | TRY expr CATCH expr FINALLY expr { $$ = TryExprNode::createExceptionBlock($2, $4, $6); }
       | TRY expr FINALLY expr            { $$ = TryExprNode::createExceptionBlock($2, nullptr, $4); }
       ;

/* --------------------- TRY --------------------- */


/* --------------------- FUNC --------------------- */

funcParamClause: nlo '(' ')'            { $$ = nullptr; }
               | nlo '(' funcParams ')' { $$ = $3; }
               ;

funcParams: funcParam                { $$ = FuncParamsNode::addFuncParamToList(nullptr, $1); }
          | funcParams ',' funcParam { $$ = FuncParamsNode::addFuncParamToList($1, $3); }
          ;

funcParam: fullID compoundType { $$ = FuncParamNode::createFuncParam($1, $2); }
         ;

/* --------------------- FUNC --------------------- */

/* --------------------- CLASS --------------------- */

classParamClause: nlo '(' ')'             { $$ = new ClassParamsNode(); }
                | nlo '(' classParams ')' { $$ = $3; }
                ;

classParams: classParam                 { $$ = ClassParamsNode::addClassParamToList(nullptr, $1); }
           | classParams ',' classParam { $$ = ClassParamsNode::addClassParamToList($1, $3); }
           ;

classParam: modifiers VAL fullID ':' compoundType { $$ = ClassParamNode::createClassParam(_VAL_CLASS_PARAM, $1, $3, $5); }
          | modifiers VAR fullID ':' compoundType { $$ = ClassParamNode::createClassParam(_VAR_CLASS_PARAM, $1, $3, $5); }
          ;

modifiers: /* empty */        { $$ = nullptr; }
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

templateBody: nlo '{' templateStats '}' { $$ = TemplateStatsNode::addFuncParamToFrontToList($3, nullptr); }
            ;

templateStats: templateStat                     { $$ = TemplateStatsNode::addFuncParamToBackToList(nullptr, $1);; }
             | templateStats semi templateStat  { $$ = TemplateStatsNode::addFuncParamToBackToList($1, $3); }
             ;

templateStat: /* empty */   { $$ = nullptr; }
            | modifiers def { $$ = TemplateStatNode::createDefTemplate($1, $2); }
            | modifiers dcl { $$ = TemplateStatNode::createDclTemplate($1, $2); }
            ;

/* --------------------- DECL --------------------- */

dcl: VAL ids ':' compoundType { $$ = DclNode::createValDcl($2, $4); }
   | VAR ids ':' compoundType { $$ = DclNode::createVarDcl($2, $4); }
   | DEF funSig compoundTypeO { $$ = DclNode::createDefDcl($2, $3); }
   ;

funSig: fullID funcParamClause { $$ = FunSigNode::createFunSig($1, $2); }
      ;

/* --------------------- DECL --------------------- */

/* --------------------- DEFS --------------------- */

varDefs: VAL ids compoundTypeO '=' expr { $$ = VarDefsNode::createVal($2, $3, $5); }
       | VAR ids compoundTypeO '=' expr { $$ = VarDefsNode::createVar($2, $3, $5); }
       ;

def: varDefs    { $$ = DefNode::createVarDefs($1); }
   | DEF funDef { $$ = DefNode::createFunDef($2); }
//   | tmplDef    { $$ = DefNode::createTmplDef($1); } // запрещает вложенные классы, трейты, перечисоения, имбо мы ебали возиться ещё и с их скоупами и таблицами
   ;

funDef: funSig compoundTypeO '=' expr       { $$ = FunDefNode::createFunSigFunDef($1, $2, $4); }
      | THIS funcParamClause '=' constrExpr { $$ = FunDefNode::createThisConstrCallFunDef($2, $4); }
      ;

constrExpr: THIS argumentExprs { $$ = ConstrExprNode::createConstrExpr($2, nullptr); } // вызов первичного конструктора, бывший selfInvocation
          | '{' THIS argumentExprs semi blockStats '}' { $$ = ConstrExprNode::createConstrExpr($3, $5, true); }
          | '{' THIS argumentExprs '}' { $$ = ConstrExprNode::createConstrExpr($3, nullptr, true); }
          ;

tmplDef: CLASS classDef                 { $$ = TemplateDefNode::createClassDef($2); }
       | OBJECT fullID classTemplateOpt { $$ = TemplateDefNode::createObjectDef($2, $3); }
       | TRAIT fullID traitTemplateOpt  { $$ = TemplateDefNode::createTraitDef($2, $3); }
       | ENUM enumDef                   { $$ = TemplateDefNode::createEnumDef($2); }
       ;

classDef: fullID accessModifier classParamClause classTemplateOpt { $$ = ClassDefNode::createClassDef($1, $2, $3, $4); }
        | fullID classParamClause classTemplateOpt                { $$ = ClassDefNode::createClassDef($1, nullptr, $2, $3); }
        | fullID classTemplateOpt                                 { $$ = ClassDefNode::createClassDef($1, nullptr, nullptr, $2); }
        ;

enumDef: fullID accessModifier classParamClause enumTemplate { $$ = EnumDefNode::createWithAccessModifier($1, $2, $3, $4); }
       | fullID classParamClause enumTemplate                { $$ = EnumDefNode::createWithClassParams($1, $2, $3); }
       | fullID enumTemplate                                 { $$ = EnumDefNode::createEnumTemplate($1, $2 ); }
       ;

classTemplateOpt: /* empty */ %prec LOW_PREC { $$ = nullptr; }
                | templateBody               { $$ = ClassTemplateOptNode::addFuncParamToBackToList($1); }
	        | EXTENDS classParents %prec END_TEMPLATE { $$ = ClassTemplateOptNode::createClassTemplate($2, nullptr); }
	        | EXTENDS classParents templateBody { $$ = ClassTemplateOptNode::createClassTemplate($2, $3); }
                ;

traitTemplateOpt: /* empty */ %prec LOW_PREC { $$ = nullptr; }
                | EXTENDS traitTemplate      { $$ = TraitTemplateOptNode::createTraitTemplateOpt($2, nullptr); }
                | templateBody               { $$ = TraitTemplateOptNode::createTraitTemplateOpt(nullptr, $1); }
                ;

enumTemplate: EXTENDS classParents enumBody { $$ = EnumTemplateNode::createWithClassParents($2, $3); }
            | enumBody                      { $$ = EnumTemplateNode::createWithEnumBody($1); }
            ;

classParents: stableId argumentExprs simpleTypes { $$ = ClassParentsNode::createClassParents(ConstrInvokeNode::createConstrInvokeNode($1, $2), $3); } // (бывший constrInvoke)
	    | stableId simpleTypes { $$ = ClassParentsNode::createClassParents(ConstrInvokeNode::createConstrInvokeNode($1, nullptr), $2); } // (бывший constrInvoke)
	    ;

traitTemplate: simpleType simpleTypes templateBody       { $$ = TraitTemplateNode::createTraitTemplate($1, $2, $3); }
             | simpleType simpleTypes %prec END_TEMPLATE { $$ = TraitTemplateNode::createTraitTemplate($1, $2, nullptr); }
             ;

enumBody: nlo '{' enumStats '}' { $$ = $3; }
	;

enumStats: enumStat                { $$ = EnumStatsNode::addModifierToList(nullptr, $1); }
	 | enumStats semi enumStat { $$ = EnumStatsNode::addModifierToList($1, $3); }
	 ;

enumStat: templateStat       { $$ = EnumStatNode::createWithTemplateStat($1); }
	| modifiers enumCase { $$ = EnumStatNode::createWithEnumCase($1, $2); }
	;

enumCase: CASE fullID classParamClause EXTENDS classParents { $$ = EnumCaseNode::createClassParents(_CASE_WITH_EXTENDS, IdsNode::addIdToList(nullptr, $2), $3, $5); }
	| CASE fullID classParamClause                      { $$ = EnumCaseNode::createClassParents(_CASE_WITH_PARAMS, IdsNode::addIdToList(nullptr, $2), $3, nullptr); }
	| CASE fullID %prec END_TEMPLATE                    { $$ = EnumCaseNode::createClassParents(_CASE_WITH_IDS, IdsNode::addIdToList(nullptr, $2), nullptr, nullptr); }
        | CASE ids ',' fullID                               { $$ = EnumCaseNode::createClassParents(_CASE_WITH_IDS, IdsNode::addIdToList($2, $4), nullptr, nullptr); } // Решает rr
	;

topStatSeq: topStat                 { $$ = TopStatSeqNode::addModifierToList(nullptr, $1); }
	  | topStatSeq semi topStat { $$ = TopStatSeqNode::addModifierToList($1, $3); }
          ;

topStat: modifiers tmplDef { $$ = TopStatNode::createTopStat($1, $2); }
       ;

simpleTypes: /* empty */                 { $$ = nullptr; }
           | simpleTypes WITH simpleType { $$ = SimpleTypesNode::addSimpleTypeToList($1, $3); }
           ;

/* --------------------- DEFS --------------------- */

literal: DECIMAL_LITERAL { $$ = SimpleExpr1Node::createIntNode($1); }
       | CHAR_LITERAL    { $$ = SimpleExpr1Node::createCharNode($1); }
       | DOUBLE_LITERAL  { $$ = SimpleExpr1Node::createDoubleNode($1); }
       | STRING_LITERAL  { $$ = SimpleExpr1Node::createStringNode($1); }
       | TRUE_LITERAL    { $$ = SimpleExpr1Node::createBoolNode($1); }
       | FALSE_LITERAL   { $$ = SimpleExpr1Node::createBoolNode($1); }
       | NULL_LITERAL    { $$ = SimpleExpr1Node::createNullNode(); }
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
      | EQUAL            { $$ = IdNode::createOperator("="); }
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