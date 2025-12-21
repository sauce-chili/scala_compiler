%{
#include <iostream>
using namespace std;

int yylex();
void yyerror(const char* s);
%}


%token NON

%token IF ELSE
%token FOR WHILE DO
%token TRY
%token CATCH
%token FINALLY
%token THROW
%token VAL VAR
%token NEW
%token RETURN
%token CLASS OBJECT DEF TRAIT
%token THIS SUPER
%token ARRAY
%token NL
%token ID
%token WITH
%token LEFT_ARROW

%token PRIVATE PROTECTED OVERRIDE ABSTRACT FINAL SEALED EXTENDS
%token INT STRING CHAR BOOLEAN UNIT
%token YIELD

%token DECIMAL_LITERAL
%token CHAR_LITERAL
%token DOUBLE_LITERAL
%token STRING_LITERAL
%token TRUE_LITERAL FALSE_LITERAL
%token NULL_LITERAL

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
%right UMINUS UPLUS '!' '~' ID_EXCLAMATION
%left '.'
%nonassoc ':' ID_COLON
%nonassoc '(' '['
%nonassoc POSTFIX_OP
%nonassoc INFIX_OP
%nonassoc RETURN_EMPTY
%nonassoc RETURN_EXPR
%nonassoc CATCH
%nonassoc FINALLY

%start scala_file

%%

scala_file: topStatSeq
          ;

expr: IF '(' expr ')' nls expr semio ELSE expr
    | IF '(' expr ')' nls expr
    | WHILE '(' expr ')' nls expr
    | tryExpr
    | DO expr semio WHILE '(' expr ')'
    | THROW expr
    | RETURN                                   %prec RETURN_EMPTY
    | RETURN expr                              %prec RETURN_EXPR
    | FOR '(' enumerators ')' nls yieldO expr
    | simpleExpr '.' fullID '=' expr
    | fullID '=' expr
    | simpleExpr1 argumentExprs '=' expr
    | infixExpr
    ;

yieldO: /* empty */
      | YIELD
      ;

enumerators: generator
           | enumerators ';' generator
           ;

generator: fullID generatorTypeO LEFT_ARROW expr generatorTail
         ;

generatorTypeO: /* empty */
              | ':' infixType
              ;

generatorTail: /* empty */
             | generatorTail semi fullID generatorTypeO '=' expr
             ;

infixExpr:
      prefixExpr infixTail
;

infixTail: /* empty */
    | infixTail fullID nls prefixExpr
;

prefixExpr: simpleExpr
          | UMINUS simpleExpr
          | UPLUS simpleExpr
          | '~' simpleExpr
          | '!' simpleExpr
          ;

simpleExpr: NEW constr
          | blockExpr
          | simpleExpr1
          ;

simpleExpr1: literal
           | path
           | '(' expr ')'
           | '(' ')'
           | simpleExpr '.' fullID
           | simpleExpr1 argumentExprs // вызов метода
           ;

argumentExprs: '(' exprs ')'
             ;

exprs: /* empty */
     | expr
     | exprs ',' expr
     ;

blockExpr: '{' block '}'
         ;

constr: simpleType
      | simpleType argumentExprs
      ;

path: stableId
    | fullID '.' THIS
    | THIS
    ;

stableId: fullID
        | fullID '.' SUPER '.' fullID
        | SUPER '.' fullID
        | fullID '.' THIS chaining
        | THIS chaining
        | stableId chaining
        ;

chaining: '.' fullID
        | chaining '.' fullID
        ;

infixType: compoundType
         | infixType fullID compoundType
         ;

compoundType: simpleType
            | compoundType WITH simpleType
            ;

simpleType: stableId
          | ARRAY '[' infixType ']'
          ;

block: blockStats
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

funcParamClause: nls '(' ')'
               | nls '(' funcParams ')'
               ;

funcParams: funcParam
          | funcParams ',' funcParam
          ;

funcParam: fullID generatorTypeO assignExprO
         ;

assignExprO: /* empty */
           | '=' expr
           ;

/* --------------------- FUNC --------------------- */

/* --------------------- CLASS --------------------- */

classParamClause: nls '(' ')'
               | nls '(' classParams ')'
               ;

classParams: classParam
          | classParams ',' classParam
          ;

classParam: modifiers VAL
          | modifiers VAR
          | fullID semi infixType
          | fullID semi infixType '=' expr
          ;

modifiers: /* empty */
         | modifiers modifier
         ;

modifier: ABSTRACT
        | FINAL
        | SEALED
        | PRIVATE
        | PROTECTED
        | OVERRIDE
        ;

/* --------------------- CLASS --------------------- */

templateBody: nls '{' templateStat templateStats
            ;

templateStats: /* empty */
             | templateStats semi templateStat
             ;

templateStat: /* empty */
            | modifiers def
            | modifiers dcl
            | expr
            ;

/* --------------------- DECL --------------------- */

dcl: VAL ids ':' infixType
   | VAR ids ':' infixType
   | DEF funDcl
   ;

funDcl: funSig generatorTypeO
      ;

funSig: fullID funcParamClause
      ;

/* --------------------- DECL --------------------- */

/* --------------------- DEFS --------------------- */

varDefs: VAL ids '=' expr
       | VAR ids '=' expr
       | VAL ids ':' infixType '=' expr
       | VAR ids ':' infixType '=' expr
       ;

def: varDefs
   | DEF funDef
   | tmplDef
   ;

funDef: funSig generatorTypeO '=' expr
      | funSig nls '{' block '}'
      | THIS funcParamClause '=' constrExpr
      | THIS funcParamClause nls constrBlock
      ;

tmplDef: CLASS fullID classParamClause classTemplateOpt
       | CLASS fullID classTemplateOpt
       | OBJECT classTemplateOpt
       | TRAIT fullID traitTemplateOpt
       ;

classTemplateOpt: /* empty */
              | EXTENDS classTemplate
              | EXTENDS templateBody
              | templateBody
              ;

traitTemplateOpt: /* empty */
                | EXTENDS traitTemplate
                | EXTENDS templateBody
                | templateBody
                ;

classTemplate: constr simpleTypes templateBody
             | constr simpleTypes
             ;

traitTemplate: simpleType simpleTypes templateBody
             | simpleType simpleTypes
             ;

constrExpr: selfInvocation
          | constrBlock
          ;

constrBlock: '{' selfInvocation blockStats '}'
           ;

selfInvocation: THIS argumentExprs
              ;

topStatSeq: topStat topStats
          ;

topStats: /* empty */
        | topStats semi topStat
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

nls: /*empty*/
	| nls NL
	;

semi: ';'
    ;

semio: /*empty*/
     | semi
     ;

fullID: '+'
      | '_'
      | '!'
      | '#'
      | '%'
      | '&'
      | '*'
      | '-'
      | '/'
      | ':'
      | '<'
      | '>'
      | '?'
      | '@'
      | '\\'
      | '^'
      | '~'
      | PLUS_ASSIGNMENT
      | MINUS_ASSIGNMENT
      | MUL_ASSIGNMENT
      | DIV_ASSIGNMENT
      | MOD_ASSIGNMENT
      | EQUAL
      | NOT_EQUAL
      | LESS_EQUAL
      | GREATER_EQUAL
      | GREATER_OR_EQUAL
      | LESS_OR_EQUAL
      | ID
      | ID_EQUALITY
      | ID_VERTICAL_SIGN
      | ID_AMPERSAND
      | ID_UPPER_ARROW
      | ID_LEFT_ARROW
      | ID_RIGHT_ARROW
      | ID_MINUS
      | ID_PLUS
      | ID_ASTERISK
      | ID_SLASH
      | ID_PERCENT
      | ID_EXCLAMATION
      | ID_COLON
      ;

%%

void yyerror(const char* s) {
    cerr << "Parser error: " << s << endl;
}