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
%token nl
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
%left '.' '(' ')'

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

expr: literal
    | ID
    | if_expr
    | while_expr
    | try_expr
    | do_while_expr
    | THROW expr
    | return_expr
    | THIS
    | SUPER
    | expr '.' ID
    | expr '.' ID '(' ')' // вызов функции
    | expr '.' ID '(' expr_seq ')' '=' expr // запись в массив
    | expr '.' ID '(' expr_seq ')' // вызов функции или обращение у массиву
    | expr OR expr
    | expr AND expr
    | '!' expr
    | '-' ele expr %prec UMINUS
    | '+' ele expr %prec UPLUS
    | '(' expr ')'
    | ID '(' ')' // вызов функции
    | ID '(' expr_seq ')' '=' expr // запись в массив
    | ID '(' expr_seq ')' // вызов функции или обращение у массиву
    | expr '=' expr
    | expr '+' expr
    | expr '-' expr
    | expr '*' expr
    | expr '/' expr
    | expr '%' expr
    | expr '<' expr
    | expr '>' expr
    | expr GREATER_OR_EQUAL expr
    | expr LESS_OR_EQUAL expr
    | expr EQUAL expr
    | expr NOT_EQUAL expr
    | expr PLUS_ASSIGNMENT expr
    | expr MINUS_ASSIGNMENT expr
    | expr MUL_ASSIGNMENT expr
    | expr DIV_ASSIGNMENT expr
    | expr MOD_ASSIGNMENT expr
    | INCREMENT ele expr
    | DECREMENT ele expr
    | expr INCREMENT %prec POST_INCREMENT
    | expr DECREMENT %prec POST_DECREMENT
    | ARRAY '[' type ']' '(' expr_seq_opt ')'
    | ARRAY '(' expr_seq_opt ')'
    ;

expr_seq_opt: /*empty*/
            | expr_seq
            ;

expr_seq: expr
    | expr_seq ',' expr
    ;


while_expr: WHILE '(' expr ')' nls expr
          ;

try_expr: TRY expr CATCH expr FINALLY expr
        | TRY expr CATCH expr
        | TRY expr
        ;

do_while_expr: DO expr semio WHILE '(' expr ')'
             ;

if_expr: IF '(' expr ')' nls expr semio ELSE expr
       | IF '(' expr ')' nls expr
       ;

return_expr: RETURN expr
           | RETURN
           ;

var_decl: var_decl_kw ID ':' nlo type nlo '=' nlo expr
        ;

var_decl_kw: VAR
           | VAL
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

nlo: /* empty */
           | nl
           ;

nls: /*empty*/
	| nls nl
	;

semi: SEMICOLON
    | nl nls
    ;

semio: /*empty*/
     | semi
     ;

%%

void yyerror(const char* s) {
    cerr << "Parser error: " << s << endl;
}