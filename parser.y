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
%token TRUE_LITERAL
%token FALSE_LITERAL
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

While_Stmt: WHILE '(' expr ')' stmt
          ;

If_Expr: IF '(' expr ')' stmt
       | IF '(' expr ')' stmt ELSE stmt
       ;

Literal: DECIMAL_LITERAL
       | CHAR_LITERAL
       | DOUBLE_LITERAL
       | STRING_LITERAL
       | TRUE_LITERAL
       | FALSE_LITERAL
       | NULL_LITERAL
       ;