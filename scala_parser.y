%{
       #include <cstdio>
       void yyerror(char const * s);
       extern int yylex(void);
%}

%token IF
%token IMPORT
%token ELSE
%token VAL
%token VAR
%token CLASS
%token PUBLIC
%token PROTECTED
%token PRIVATE
%token INTERNAL
%token WHILE
%token DO
%token ID
%token THIS
%token INT
%token STRING
%token CHAR
%token BOOLEAN
%token UNIT
%token VOID
%token MATCH
%token NEW
%token CASE
%token RETURN
%token THEN
%token YIELD
%token FOR
%token DEF
%token OBJECT
%token ENUM
%token EXTENDS
%token OVERRIDE
%token ABSTRACT
%token SUPER
%token DOUBLE
%token NL
%token _

%token DECIMAL_LITERAL
%token CHAR_LITERAL
%token DOUBLE_LITERAL
%token STRING_LITERAL
%token BOOLEAN_LITERAL
%token NULL_LITERAL

%right '=' PLUS_ASSIGNMENT MINUS_ASSIGNMENT MUL_ASSIGNMENT DIV_ASSIGNMENT MOD_ASSIGNMENT
%left EQUAL NOT_EQUAL
%left '>' '<' GREATER_OR_EQUAL LESS_OR_EQUAL
%left '+' '-'
%left '*' '/' '%'
%left UMINUS UPLUS
%left '.'
%left '|' '^' '&'
%nonassoc '(' ')'

%%

stmtList: stmt
        | stmtList stmt
        ;

blockStmt: '{' nl_optional stmtList '}'
         ;

stmt: IMPORT
    | expr NL
    ;

exprList: expr
        | exprList expr
        ;

expr: '(' expr ')'
    | literal
    | ID
    | expr '.' nl_optional ID
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
    | expr '=' nl_optional expr
    | whileExpr
    | '-' expr %prec UMINUS
    | '+' expr %prec UPLUS
    ;

exprList: expr
        | exprList expr
        ;

blockExpr: '{' nl_optional exprList nl_optional '}'
         ;

ifExpr: IF nl_optional '(' nl_optional expr nl_optional ')' nl_optional expr
      | IF nl_optional '(' nl_optional expr nl_optional ')' nl_optional blockExpr nl_optional ELSE nl_optional blockExpr
      ;

whileExpr: WHILE nl_optional '(' nl_optional expr nl_optional ')' nl_optional stmt
         | WHILE nl_optional '(' nl_optional expr nl_optional ')' nl_optional blockStmt
         ;

var_decl_kw: VAR
           | VAL
           ;

type: default_type
    | ID
    ;

default_type: INT
            | DOUBLE
            | STRING
            | CHAR
            | BOOLEAN
            | UNIT
            | VOID
            ;

literal: DECIMAL_LITERAL
       | DOUBLE_LITERAL
       | CHAR_LITERAL
       | STRING_LITERAL
       | BOOLEAN_LITERAL
       | NULL_LITERAL
       ;

nl_optional: /* empty */
           | nl
           ;

nl: NL
	| nl NL
	;

%%

void yyerror(char const * s)
{
       printf("%s\n", s);
}