%token IF
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
%nonassoc '(' ')'

%%

expr_list_empty: /* empty */
               | expr_list
               ;

expr_list: expr
         | expr_list expr
         | expr_list ';' expr
         ;

expr: nl_optional expr nl_optional
    | '(' expr ')'
    | literal
    | ID
    | expr '.' nl_optional ID
    | expr '.' nl_optional ID nl_optional '(' expr_list_empty ')'
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
    | expr MUL_ASSIGNMENT expr
    | expr MUL_ASSIGNMENT expr
    | '-' nl_optional %prec UMINUS
    | '-' nl_optional %prec UPLUS
    ;

while_expr: WHILE nl_optional '(' expr ')' nl_optional stmt
          ;

if_expr: IF nl_optional '(' expr ')' nl_optional stmt
       | IF nl_optional '(' expr ')' nl_optional stmt nl_optional ELSE nl_optional stmt
       ;

/* Может использоваться в for */
if_without_stmt_expr: IF nl_optional '(' expr ')'
                    | IF expr
                    ;

var_decl: var_decl_kw nl_optional ID nl_optional ':' nl_optional type nl_optional '=' nl_optional _
        | var_decl_kw nl_optional ID nl_optional ':' nl_optional type nl_optional '=' expr
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
            | VOID

literal: DECIMAL_LITERAL
       | CHAR_LITERAL
       | DOUBLE_LITERAL
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