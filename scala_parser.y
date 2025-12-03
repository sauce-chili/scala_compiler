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
%right UMINUS UPLUS '!'
%right '=' PLUS_ASSIGNMENT MINUS_ASSIGNMENT MUL_ASSIGNMENT DIV_ASSIGNMENT MOD_ASSIGNMENT
%left OR
%left AND
%left EQUAL NOT_EQUAL
%left '<' '>' LESS_EQUAL GREATER_EQUAL
%left GREATER_OR_EQUAL LESS_OR_EQUAL
%left TO UNTIL
%left '+' '-'
%left '*' '/' '%'
%nonassoc ':'
%left '.'

%left CATCH FINALLY
%nonassoc TRY_CLAUSE_EMPTY

//%start scala_file

%%

//scala_file: top_stat_seq

expr_list: expr
         | expr_list expr
         | expr_list ';' expr
         ;

expr: literal
    | fullID
    | while_expr
    | tryExpr
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

block: blockStat
     | blockStat ';' blockStat
     | blockStat ';' block
     | blockStat ';' blockStat expr
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

expr_seq_opt: /*empty*/
            | expr_seq
            ;

expr_seq: expr
    | expr_seq ',' expr
    ;


while_expr: WHILE '(' expr ')' nls expr
          ;

tryExpr: TRY expr try_clauses
        ;

try_clauses: CATCH expr FINALLY expr
           | CATCH expr
           | FINALLY expr
           | /* empty */ %prec TRY_CLAUSE_EMPTY
           ;

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