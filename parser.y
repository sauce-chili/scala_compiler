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
%left '>' '<' GREATER_EQUAL LESS_EQUAL
%left IN
%left RANGE
%left '+' '-'
%left '*' '/' '%'
%left UMINUS UPLUS
%right PREF_INCREMENT PREF_DECREMENT
%left POST_INCREMENT POST_DECREMENT '.'
%nonassoc '(' ')'

%%

expr: LITERAL
    | ID
    |

simple_expression: LITERAL
                 | ID
                 | '(' simple_expression ')'
                 | simple_expression '.' ID
                 | simple_expression '.' ID '('

arithmetical_expression:


while_expr: WHILE '(' expr ')' stmt
          ;

if_expr: IF '(' expr ')' stmt
       | IF '(' expr ')' stmt ELSE stmt
       ;

/* Может использоваться в for */
if_without_stmt_expr: IF '(' expr ')'
                    | IF expr
                    ;

var_decl: var_decl_kw ID ':' type '=' _
        | var_decl_kw ID ':' type '=' expr
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

nl: NL
	| nl NL
	;