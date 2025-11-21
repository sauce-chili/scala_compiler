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
%token VAL VAR
%token NEW
%token RETURN
%token CLASS OBJECT DEF
%token THIS SUPER
%token ARRAY
%token NL
%token SEMICOLON
%token ID

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
%right UMINUS UPLUS INCREMENT DECREMENT '!'
%left '.' '[' ']' '(' ')'

//%start scala_file

%%

//scala_file: top_stat_seq

exp_list_opt: /* empty */
               | expr_list
               ;

expr_list: expr
         | expr_list expr
         | expr_list ';' expr
         ;

expr: nlo expr nlo
    | '(' expr ')'
    | literal
    | ID
    | expr '.' nlo ID
    | expr '.' nlo ID nlo '(' exp_list_opt ')'
    | expr '+' expr
    | expr '-' expr
    | expr '*' expr
    | expr '/' expr
    | expr '%' expr
    | expr '<' expr
    | expr '>' expr
    | expr GREATER_OR_EQUAL expr
    | expr LESS_OR_EQUAL expr
    | expr PLUS_ASSIGNMENT expr
    | expr MINUS_ASSIGNMENT expr
    | expr MUL_ASSIGNMENT expr
    | expr DIV_ASSIGNMENT expr
    | expr MOD_ASSIGNMENT expr
    | expr '=' nlo expr
    | expr MUL_ASSIGNMENT expr
    | expr MUL_ASSIGNMENT expr
    | '-' nlo %prec UMINUS
    | '-' nlo %prec UPLUS
    ;

while_expr: WHILE nlo '(' expr ')' nlo expr
          ;

try_expr: TRY nlo expr
        | TRY nlo expr nlo CATCH nlo expr
        | TRY nlo expr nlo CATCH nlo expr nlo FINALLY nlo expr

do_while_expr: DO nlo expr nlo WHILE nlo '(' expr ')'
             | DO nlo expr semi WHILE nlo '(' expr ')'

if_expr: IF nlo '(' expr ')' nlo expr
       | IF nlo '(' expr ')' nlo expr nlo ELSE nlo expr
       | IF nlo '(' expr ')' nlo expr semi ELSE nlo expr
       ;

var_decl: var_decl_kw nlo ID nlo ':' nlo type nlo '=' expr
        ;

var_decl_kw: VAR
           | VAL
           ;

type: default_type
    | ID
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

nlo: /* empty */
           | nl
           ;

nl: NL
	| nl NL
	;

semi: SEMICOLON
    | nl nlo
    ;