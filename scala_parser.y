%code requires {
    #include "nodes/modifiers/ModifierNode.h"
    #include "nodes/modifiers/ModifiersNode.h"
}

%code {
    #include <iostream>
    using namespace std;

    int yylex();
    void yyerror(const char* s);
}

%parse-param { ScalaFileNode** out_root }

%union {
    int intLiteral;
    char charLiteral;
    double doubleLiteral;
    char* stringLiteral;
    bool boolLiteral;
    char* identifier;
    char* unitLiteral;

    ModifierNode* modifier;
    ModifiersNode* modifiers;

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
%token ID

%token PRIVATE PROTECTED OVERRIDE ABSTRACT FINAL SEALED EXTENDS
%token INT DOUBLE STRING CHAR BOOLEAN UNIT ARRAY

%token DECIMAL_LITERAL
%token CHAR_LITERAL
%token DOUBLE_LITERAL
%token STRING_LITERAL
%token TRUE_LITERAL FALSE_LITERAL
%token NULL_LITERAL

%token NL

%nonassoc LOW_PREC
%nonassoc RETURN IF FOR NL
%nonassoc ELSE WHILE DO TRY THROW VAL VAR NEW YIELD MATCH CASE
%right '=' LEFT_ARROW PLUS_ASSIGNMENT MINUS_ASSIGNMENT MUL_ASSIGNMENT DIV_ASSIGNMENT MOD_ASSIGNMENT ID_EQUALITY
%left OR '|' ID_VERTICAL_SIGN
%left AND '&' ID_AMPERSAND
%left '^' ID_UPPER_ARROW
%left EQUAL NOT_EQUAL
%left '<' '>' LESS_EQUAL GREATER_EQUAL GREATER_OR_EQUAL LESS_OR_EQUAL ID_LEFT_ARROW ID_RIGHT_ARROW
%left TO UNTIL
%left '+' '-' ID_MINUS ID_PLUS
%left '*' '/' '%' ID_ASTERISK ID_SLASH ID_PERCENT
%left UMINUS UPLUS ID '#' '?' '@' '\\' '!' '~' ID_EXCLAMATION
%left '.'
%nonassoc ':' ID_COLON
%nonassoc '(' '['
%nonassoc CATCH
%nonassoc FINALLY
%nonassoc END_TEMPLATE


%start scalaFile

%%

scalaFile: topStatSeq
          ;

expr: IF '(' expr ')' nls expr semio ELSE expr
    | IF '(' expr ')' nls expr
    | WHILE '(' expr ')' nls expr
    | tryExpr
    | DO expr semio WHILE '(' expr ')'
    | THROW expr
    | RETURN
    | RETURN expr
    | FOR '(' enumerators ')' nls yieldO expr
    | infixExpr
    | assignment
    ;

assignment: fullID '=' expr
          | simpleExpr '.' fullID '=' expr
          | simpleExpr1 argumentExprs '=' expr // запись в массив
          ;

yieldO: /* empty */
      | YIELD
      ;

enumerators: generator
           | enumerators semi enumeratorPart
           ;

enumeratorPart: generator
              | fullID compoundTypeO '=' expr // определение переменной
              ;

generator: fullID compoundTypeO LEFT_ARROW expr
         ;

compoundTypeO: /* empty */ %prec LOW_PREC
          | ':' compoundType
          ;

infixExpr: prefixExpr
         | infixExpr '+' nlo infixExpr
         | infixExpr '!' nlo infixExpr
	 | infixExpr '#' nlo infixExpr
         | infixExpr '%' nlo infixExpr
	 | infixExpr '&' nlo infixExpr
	 | infixExpr '*' nlo infixExpr
	 | infixExpr '-' nlo infixExpr
	 | infixExpr '/' nlo infixExpr
	 | infixExpr ':' nlo infixExpr
	 | infixExpr '<' nlo infixExpr
	 | infixExpr '>' nlo infixExpr
	 | infixExpr '?' nlo infixExpr
	 | infixExpr '@' nlo infixExpr
	 | infixExpr '\\' nlo infixExpr
	 | infixExpr '^' nlo infixExpr
	 | infixExpr '~' nlo infixExpr
	 | infixExpr PLUS_ASSIGNMENT nlo infixExpr
	 | infixExpr MINUS_ASSIGNMENT nlo infixExpr
	 | infixExpr MUL_ASSIGNMENT nlo infixExpr
	 | infixExpr DIV_ASSIGNMENT nlo infixExpr
	 | infixExpr MOD_ASSIGNMENT nlo infixExpr
	 | infixExpr EQUAL nlo infixExpr
	 | infixExpr NOT_EQUAL nlo infixExpr
	 | infixExpr LESS_EQUAL nlo infixExpr
	 | infixExpr GREATER_EQUAL nlo infixExpr
	 | infixExpr GREATER_OR_EQUAL nlo infixExpr
	 | infixExpr LESS_OR_EQUAL nlo infixExpr
	 | infixExpr ID nlo infixExpr
	 | infixExpr ID_EQUALITY nlo infixExpr
	 | infixExpr ID_VERTICAL_SIGN nlo infixExpr
	 | infixExpr ID_AMPERSAND nlo infixExpr
	 | infixExpr ID_UPPER_ARROW nlo infixExpr
	 | infixExpr ID_LEFT_ARROW nlo infixExpr
	 | infixExpr ID_RIGHT_ARROW nlo infixExpr
	 | infixExpr ID_MINUS nlo infixExpr
	 | infixExpr ID_PLUS nlo infixExpr
	 | infixExpr ID_ASTERISK nlo infixExpr
	 | infixExpr ID_SLASH nlo infixExpr
	 | infixExpr ID_PERCENT nlo infixExpr
	 | infixExpr ID_EXCLAMATION nlo infixExpr
	 | infixExpr ID_COLON nlo infixExpr
         ;

prefixExpr: simpleExpr
          | UMINUS simpleExpr
          | UPLUS simpleExpr
          | '~' simpleExpr
          | '!' simpleExpr
          ;

simpleExpr: NEW constrInvoke
          | '{' blockStats '}'
          | simpleExpr1
          ;

simpleExpr1: literal
           | fullID
           | SUPER '.' fullID
           | THIS '.' fullID
           | simpleExpr '.' fullID
           | '(' expr ')'
           | '(' ')'
           | simpleExpr1 argumentExprs // вызов метода
           ;

argumentExprs: '(' exprs ')'
             ;

exprs: /* empty */
     | expr
     | exprs ',' expr
     ;

constrInvoke: simpleType argumentExprs // бывший constr
      	    ;

compoundType: simpleType
            | compoundType WITH simpleType
            ;

simpleType: stableId
          | ARRAY '[' compoundType ']'
          ;

stableId: fullID
        | SUPER '.' fullID
        | THIS '.' fullID
        | stableId '.' fullID
        ;

blockStats: blockStat
          | blockStats semi blockStat
          ;

blockStat: varDefs
         | expr
         ;

ids: fullID
   | ids ',' fullID
   ;

/* --------------------- TRY --------------------- */

tryExpr: TRY expr
       | TRY expr CATCH expr
       | TRY expr CATCH expr FINALLY expr
       | TRY expr FINALLY expr
       ;

/* --------------------- TRY --------------------- */


/* --------------------- FUNC --------------------- */

funcParamClause: nlo '(' ')'
               | nlo '(' funcParams ')'
               ;

funcParams: funcParam
          | funcParams ',' funcParam
          ;

funcParam: fullID compoundTypeO assignExprO
         ;

assignExprO: /* empty */
           | '=' expr
           ;

/* --------------------- FUNC --------------------- */

/* --------------------- CLASS --------------------- */

classParamClause: nlo '(' ')'
                | nlo '(' classParams ')'
                ;

classParams: classParam
           | classParams ',' classParam
           ;

classParam: modifiers VAL fullID ':' compoundType assignExprO
          | modifiers VAR fullID ':' compoundType assignExprO
          | modifiers fullID ':' compoundType assignExprO
          ;

modifiers: /* empty */        { $$ = ModifiersNode::addModifierToList(nullptr, nullptr); }
         | modifiers modifier { $$ = ModifiersNode::addModifierToList($1, $2); }
         ;

modifier: ABSTRACT       { $$ = ModifierNode::createModifier($1); }
        | FINAL          { $$ = ModifierNode::createModifier($1); }
        | SEALED         { $$ = ModifierNode::createModifier($1); }
        | accessModifier { $$ = ModifierNode::createModifier($1); }
        | OVERRIDE       { $$ = ModifierNode::createModifier($1); }
        ;

accessModifier: PRIVATE   { $$ = ModifierNode::createModifier($1); }
	      | PROTECTED { $$ = ModifierNode::createModifier($1); }
	      ;

/* --------------------- CLASS --------------------- */

templateBody: nlo '{' templateStat templateStats '}'
            ;

templateStats: /* empty */
             | templateStats semi templateStat
             ;

templateStat: /* empty */
            | modifiers def
            | modifiers dcl
            ;

/* --------------------- DECL --------------------- */

dcl: VAL ids ':' compoundType
   | VAR ids ':' compoundType
   | DEF funSig compoundTypeO
   ;

funSig: fullID funcParamClause
      ;

/* --------------------- DECL --------------------- */

/* --------------------- DEFS --------------------- */

varDefs: VAL ids compoundTypeO '=' expr
       | VAR ids compoundTypeO '=' expr
       ;

def: varDefs
   | DEF funDef
   | tmplDef
   ;

funDef: funSig compoundTypeO '=' expr
      | THIS funcParamClause '=' constrExpr
      ;

tmplDef: CLASS classDef
       | OBJECT fullID classTemplateOpt
       | TRAIT fullID traitTemplateOpt
       | ENUM enumDef
       ;

classDef: fullID accessModifier classParamClause classTemplateOpt
        | fullID classParamClause classTemplateOpt
        | fullID classTemplateOpt
        ;

enumDef: fullID accessModifier classParamClause enumTemplate
       | fullID classParamClause enumTemplate
       | fullID enumTemplate
       ;

classTemplateOpt: /* empty */ %prec LOW_PREC
                | EXTENDS classTemplate
                | templateBody
                ;

traitTemplateOpt: /* empty */ %prec LOW_PREC
                | EXTENDS traitTemplate
                | templateBody
                ;

enumTemplate: EXTENDS classParents enumBody
            | enumBody
            ;

classTemplate: classParents templateBody
             | classParents %prec END_TEMPLATE
             ;

classParents: constrInvoke simpleTypes;

traitTemplate: simpleType simpleTypes templateBody
             | simpleType simpleTypes %prec END_TEMPLATE
             ;

enumBody: nlo '{' enumStats '}';

enumStats: enumStat
	 | enumStats semi enumStat
	 ;

enumStat: templateStat
	| modifiers enumCase
	;


enumCase: CASE fullID classParamClause EXTENDS classParents
	| CASE fullID classParamClause
	| CASE fullID %prec END_TEMPLATE
        | CASE ids ',' fullID // Решает rr
	;

constrExpr: selfInvocation
          | constrBlock
          ;

constrBlock: '{' selfInvocation blockStats '}'
           ;

selfInvocation: THIS argumentExprs
              ;

topStatSeq: topStat
	  | topStatSeq semi topStat
          ;

topStat: modifiers tmplDef
       ;

simpleTypes: /* empty */
           | simpleTypes WITH simpleType
           ;

/* --------------------- DEFS --------------------- */

literal: DECIMAL_LITERAL
       | CHAR_LITERAL
       | DOUBLE_LITERAL
       | STRING_LITERAL
       | TRUE_LITERAL
       | FALSE_LITERAL
       | NULL_LITERAL
       ;

nls: /* empty */
   | nls NL
   ;

nlo: /* empty */
   | NL
   ;

semi: ';'
    | NL nls
    ;

semio: /* empty */
     | semi
     ;

fullID: '+'              { $$ = IdNode::createId($1); }
      | '!'              { $$ = IdNode::createId($1); }
      | '#'              { $$ = IdNode::createId($1); }
      | '%'              { $$ = IdNode::createId($1); }
      | '&'              { $$ = IdNode::createId($1); }
      | '*'              { $$ = IdNode::createId($1); }
      | '-'              { $$ = IdNode::createId($1); }
      | '/'              { $$ = IdNode::createId($1); }
      | ':'              { $$ = IdNode::createId($1); }
      | '<'              { $$ = IdNode::createId($1); }
      | '>'              { $$ = IdNode::createId($1); }
      | '?'              { $$ = IdNode::createId($1); }
      | '@'              { $$ = IdNode::createId($1); }
      | '\\'             { $$ = IdNode::createId($1); }
      | '^'              { $$ = IdNode::createId($1); }
      | '~'              { $$ = IdNode::createId($1); }
      | PLUS_ASSIGNMENT  { $$ = IdNode::createId($1); }
      | MINUS_ASSIGNMENT { $$ = IdNode::createId($1); }
      | MUL_ASSIGNMENT   { $$ = IdNode::createId($1); }
      | DIV_ASSIGNMENT   { $$ = IdNode::createId($1); }
      | MOD_ASSIGNMENT   { $$ = IdNode::createId($1); }
      | EQUAL            { $$ = IdNode::createId($1); }
      | NOT_EQUAL        { $$ = IdNode::createId($1); }
      | LESS_EQUAL       { $$ = IdNode::createId($1); }
      | GREATER_EQUAL    { $$ = IdNode::createId($1); }
      | GREATER_OR_EQUAL { $$ = IdNode::createId($1); }
      | LESS_OR_EQUAL    { $$ = IdNode::createId($1); }
      | ID               { $$ = IdNode::createId($1); }
      | ID_EQUALITY      { $$ = IdNode::createId($1); }
      | ID_VERTICAL_SIGN { $$ = IdNode::createId($1); }
      | ID_AMPERSAND     { $$ = IdNode::createId($1); }
      | ID_UPPER_ARROW   { $$ = IdNode::createId($1); }
      | ID_LEFT_ARROW    { $$ = IdNode::createId($1); }
      | ID_RIGHT_ARROW   { $$ = IdNode::createId($1); }
      | ID_MINUS         { $$ = IdNode::createId($1); }
      | ID_PLUS          { $$ = IdNode::createId($1); }
      | ID_ASTERISK      { $$ = IdNode::createId($1); }
      | ID_SLASH         { $$ = IdNode::createId($1); }
      | ID_PERCENT       { $$ = IdNode::createId($1); }
      | ID_EXCLAMATION   { $$ = IdNode::createId($1); }
      | ID_COLON         { $$ = IdNode::createId($1); }
      ;

%%

void yyerror(const char* s) {
    cerr << "Parser error: " << s << endl;
}