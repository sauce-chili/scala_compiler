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
%token CLASS OBJECT DEF
%token THIS SUPER
%token ARRAY
%token NL
%token ID
%token WITH
%token LEFT_ARROW

%token PRIVATE PROTECTED OVERRIDE ABSTRACT
%token INT STRING CHAR BOOLEAN UNIT
%token MATCH
%token CASE
%token YIELD

%token DECIMAL_LITERAL
%token CHAR_LITERAL
%token DOUBLE_LITERAL
%token STRING_LITERAL
%token TRUE_LITERAL FALSE_LITERAL
%token NULL_LITERAL

%nonassoc RETURN IF FOR NL
%left '(' '['
%right '=' PLUS_ASSIGNMENT MINUS_ASSIGNMENT MUL_ASSIGNMENT DIV_ASSIGNMENT MOD_ASSIGNMENT
%left OR
%left AND
%left EQUAL NOT_EQUAL
%left '<' '>' LESS_EQUAL GREATER_EQUAL
%left GREATER_OR_EQUAL LESS_OR_EQUAL
%left TO UNTIL
%left '+' '-'
%left '*' '/' '%'
%right UMINUS UPLUS '!' '~'
%nonassoc ':'
%left '.'
%left LEFT_ARROW
%left '|'
%left '^'
%left '&'

%nonassoc LOWEST
%nonassoc CATCH
%nonassoc FINALLY
%nonassoc HIGHEST

%nonassoc POSTFIX_OP
%nonassoc INFIX_OP

%nonassoc RETURN_EMPTY
%nonassoc RETURN_EXPR

%start scala_file

%%

scala_file: expr_list

expr_list: expr
         | expr_list expr
         | expr_list ';' expr
         ;

expr: literal
    | fullID
    | if_expr
    | while_expr
    | tryExpr
    | do_while_expr
    | THROW expr
    | RETURN                                   %prec RETURN_EMPTY
    | RETURN expr                              %prec RETURN_EXPR
    | FOR '(' enumerators ')' nls yieldO expr
    | FOR '{' enumerators '}' nls yieldO expr
    ;

forExpr: FOR '(' enumerators ')' nls yieldO expr
       | FOR '{' enumerators '}' nls yieldO expr
       ;

yieldO: /* empty */
      | YIELD
      ;

enumerators: generator
           | enumerators ';' generator
           ;

generator: fullID generatorTypeO LEFT_ARROW expr generatorTailO
         ;

generatorTypeO: /* empty */
                | ':' infixType
                ;

generatorTailO: /* empty */
              | generatorTail
              ;

generatorTail: semio guard
             | generatorTail semio guard
             | generatorTail semi fullID generatorTypeO '=' expr
             ;

guard: IF postfixExpr
     ;

postfixExpr: infixExpr
           | infixExpr fullID nls %prec POSTFIX_OP
           ;

infixExpr: prefixExpr
         | infixExpr fullID nls infixExpr %prec INFIX_OP
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
           | path argumentExprs
           |

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
        | path '.' fullID
        | fullID '.' SUPER '.' fullID
        | SUPER '.' fullID
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

block: blockStat blockRecursive
     | blockStat blockRecursive expr
     ;

blockRecursive: /* empty */
              | blockRecursive semi blockStat
              ;

blockStat: varDefs
         | expr
         ;

varDefs: VAL ids '=' expr
       | VAR ids '=' expr
       | VAL ids ':' infixType '=' expr
       | VAR ids ':' infixType '=' expr
       ;

ids: fullID
   | ids ',' fullID
   ;

while_expr: WHILE '(' expr ')' nls expr
          ;

/* --------------------- TRY --------------------- */

tryExpr: TRY expr tryTail
       ;

tryTail: /* empty */ %prec LOWEST
       | CATCH expr finallyPart %prec CATCH
       | FINALLY expr %prec FINALLY
       ;

finallyPart: /* empty */ %prec LOWEST
            | FINALLY expr %prec FINALLY
            ;

/* --------------------- TRY --------------------- */

do_while_expr: DO expr semio WHILE '(' expr ')'
             ;

if_expr: IF '(' expr ')' nls expr semio ELSE expr
       | IF '(' expr ')' nls expr
       ;

type: default_type
    | ID
    | ARRAY '[' type ']'
    ;

default_type: INT
            | STRING
            | CHAR
            | BOOLEAN
            | UNIT
            ;

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
      | '='
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
      ;

%%

void yyerror(const char* s) {
    cerr << "Parser error: " << s << endl;
}