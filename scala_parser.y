%code requires {
    #include "nodes/nodes.h"
    class TopStatSeqNode;
}

%code {
    #include <iostream>
    #include <cstdio>

    // Просто объявляем функции без extern "C"
    int yylex();
    void yyerror(TopStatSeqNode** out_root, const char* s);

    using namespace std;

    const char* get_bison_token_name(int token);
}

/* для включения отладки */
%define parse.trace
%define parse.error verbose
%token-table

/* Передаем указатель на корень AST */
%parse-param { TopStatSeqNode** out_root }

%union {
    int intLiteral;
    char charLiteral;
    double doubleLiteral;
    char* stringLiteral;
    bool boolLiteral;
    char* identifier;
    char* unitLiteral;

    ClassDefNode* classDef;
    ClassParamNode* classParam;
    ClassParamsNode* classParams;
    ClassParentsNode* classParents;
    ClassTemplateNode* classTemplate;
    DclNode* dcl;
    DefNode* def;
    EnumCaseNode* enumCase;
    EnumDefNode* enumDef;
    EnumStatNode* enumStat;
    EnumStatsNode* enumStats;
    ArgumentExprsNode* argumentExprs;
    AssignmentNode* assignment;
    ConstrExprNode* constrExpr;
    ExprNode* expr;
    ExprsNode* exprs;
    InfixExprNode* infixExpr;
    PrefixExprNode* prefixExpr;
    SimpleExpr1Node* simpleExpr1;
    SimpleExprNode* simpleExpr;
    FuncParamNode* funcParam;
    FuncParamsNode* funcParams;
    FunDefNode* funDef;
    FunSigNode* funSig;
    EnumeratorPartNode* enumeratorPart;
    EnumeratorsNode* enumerators;
    GeneratorNode* generator;
    IdNode* id;
    IdsNode* ids;
    ModifierNode* modifier;
    ModifiersNode* modifiers;
    BlockStatNode* blockStat;
    BlockStatsNode* blockStats;
    TopStatNode* topStat;
    TopStatSeqNode* topStatSeq;
    ClassTemplateOptNode* classTemplateOpt;
    EnumTemplateNode* enumTemplate;
    TemplateStatNode* templateStat;
    TemplateStatsNode* templateStats;
    TraitTemplateNode* traitTemplate;
    TraitTemplateOptNode* traitTemplateOpt;
    SimpleTypeNode* simpleType;
    SimpleTypesNode* simpleTypes;
    VarDefsNode* varDefs;
}


%token NON

%token IF ELSE
%token FOR WHILE DO
%token LEFT_ARROW
%token YIELD
%token THROW
%token VAL VAR
%token NEW
%token RETURN
%token CLASS OBJECT DEF TRAIT ENUM
%token THIS SUPER

%token PRIVATE PROTECTED OVERRIDE ABSTRACT FINAL SEALED EXTENDS
%token INT DOUBLE STRING CHAR BOOLEAN UNIT ARRAY

%token DECIMAL_LITERAL
%token CHAR_LITERAL
%token DOUBLE_LITERAL
%token STRING_LITERAL
%token TRUE_LITERAL
%token FALSE_LITERAL
%token NULL_LITERAL

//%token <identifier> PLUS_ASSIGNMENT MINUS_ASSIGNMENT MUL_ASSIGNMENT DIV_ASSIGNMENT MOD_ASSIGNMENT EQUAL NOT_EQUAL GREATER_OR_EQUAL LESS_OR_EQUAL
//%token <identifier> ID ID_VERTICAL_SIGN ID_CIRCUMFLEX ID_AMPERSAND ID_EQUALITY ID_LESS ID_GREAT ID_COLON ID_MINUS ID_PLUS ID_ASTERISK ID_SLASH ID_PERCENT ID_EXCLAMATION ID_TILDE

%token NL

%nonassoc LOW_PREC
%nonassoc RETURN IF FOR NL
%nonassoc ELSE WHILE DO VAL VAR NEW YIELD MATCH CASE
%right '=' PLUS_ASSIGNMENT MINUS_ASSIGNMENT MUL_ASSIGNMENT DIV_ASSIGNMENT MOD_ASSIGNMENT
%left '|' ID_VERTICAL_SIGN
%left '^' ID_CIRCUMFLEX
%left '&' ID_AMPERSAND
%left EQUAL NOT_EQUAL ID_EQUALITY '!' ID_EXCLAMATION
%left '<' '>' GREATER_OR_EQUAL LESS_OR_EQUAL ID_LESS ID_GREAT
%right ID_COLON
%left '+' '-' ID_MINUS ID_PLUS
%left '*' '/' '%' ID_ASTERISK ID_SLASH ID_PERCENT
%left '~' ID_TILDE
%left ID '#' '?' '@' '\\'
%right UMINUS UPLUS
%right ULOGNOT UBINNOT
%left '.'
%nonassoc ':'
%nonassoc '(' '['
%nonassoc END_TEMPLATE

//%type <topStatSeq> topStatSeq
//%type <expr> expr
//%type <assignment> assignment
//%type <enumerators> enumerators
//%type <enumeratorPart> enumeratorPart
//%type <generator> generator
//%type <infixExpr> infixExpr
//%type <prefixExpr> prefixExpr
//%type <simpleExpr> simpleExpr
//%type <simpleExpr1> simpleExpr1 literal
//%type <argumentExprs> argumentExprs
//%type <exprs> exprs
//%type <simpleType> simpleType
//%type <blockStats> blockStats
//%type <blockStat> blockStat
//%type <ids> ids
//%type <funcParams> funcParamClause funcParams
//%type <funcParam> funcParam
//%type <classParams> classParamClause classParams
//%type <classParam> classParam
//%type <modifiers> modifiers
//%type <modifier> modifier accessModifier
//%type <templateStats> templateStats templateBody
//%type <templateStat> templateStat
//%type <dcl> dcl
//%type <funSig> funSig
//%type <varDefs> varDefs
//%type <def> def
//%type <funDef> funDef
//%type <constrExpr> constrExpr
//%type <classDef> classDef
//%type <classTemplateOpt> classTemplateOpt
//%type <classTemplate> classTemplate
//%type <classParents> classParents
//%type <topStat> topStat
//%type <id> fullID


%start scalaFile


%%

scalaFile: topStatSeq 
	 ;

expr: IF '(' expr ')' nls expr ELSE expr 
    | IF '(' expr ')' nls expr           	
    | WHILE '(' expr ')' nls expr              
    | DO expr semio WHILE '(' expr ')'         
    | RETURN                                   
    | RETURN expr                              
    | FOR '(' enumerators ')' nls expr         
    | FOR '(' enumerators ')' nls YIELD expr   
    | FOR '{' enumerators '}' nls expr         
    | FOR '{' enumerators '}' nls YIELD expr   
    | infixExpr                                
    | assignment                               
    ;

assignment: fullID '=' expr                    
          | simpleExpr '.' fullID '=' expr     
          | simpleExpr1 argumentExprs '=' expr  // запись в массив
          ;

enumerators: generator                       
           | enumerators semi enumeratorPart 
           ;

enumeratorPart: generator                     
              | fullID simpleTypeO '=' expr  // определение переменной
              ;

generator: fullID simpleTypeO LEFT_ARROW expr 
         ;

simpleTypeO: /* empty */                
          | ':' simpleType              
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
	 | infixExpr GREATER_OR_EQUAL nlo infixExpr 
	 | infixExpr LESS_OR_EQUAL nlo infixExpr    
	 | infixExpr ID nlo infixExpr               
	 | infixExpr ID_EQUALITY nlo infixExpr      
	 | infixExpr ID_VERTICAL_SIGN nlo infixExpr 
	 | infixExpr ID_AMPERSAND nlo infixExpr     
	 | infixExpr ID_CIRCUMFLEX nlo infixExpr    
	 | infixExpr ID_LESS nlo infixExpr          
	 | infixExpr ID_GREAT nlo infixExpr         
	 | infixExpr ID_MINUS nlo infixExpr         
	 | infixExpr ID_PLUS nlo infixExpr          
	 | infixExpr ID_ASTERISK nlo infixExpr      
	 | infixExpr ID_SLASH nlo infixExpr         
	 | infixExpr ID_PERCENT nlo infixExpr       
	 | infixExpr ID_EXCLAMATION nlo infixExpr   
	 | infixExpr ID_TILDE nlo infixExpr 	    
	 | infixExpr ID_COLON nlo infixExpr         
         ;

prefixExpr: simpleExpr 
          | '+' simpleExpr %prec UPLUS 
          | '-' simpleExpr %prec UMINUS 
          | '~' simpleExpr %prec UBINNOT 
          | '!' simpleExpr %prec ULOGNOT 
          ;

simpleExpr: NEW fullID argumentExprs
	  | NEW ARRAY '[' simpleType ']' argumentExprs
	  | NEW ARRAY argumentExprs // argumentExprs принимает только 1 аргумент (размер массива)
          | '{' blockStats '}' // бывший blockExpr
          | simpleExpr1
          ;


simpleExpr1: literal                   
           | fullID                    
           | SUPER '.' fullID          
           | THIS                      
           | simpleExpr '.' THIS       
           | simpleExpr '.' fullID     
           | '(' expr ')'              
           | '(' ')'                   
           | simpleExpr1 argumentExprs  // вызов метода
           | ARRAY '[' simpleType ']' argumentExprs 
           | ARRAY argumentExprs       
           ;

argumentExprs: '(' exprs ')' 
             ;

exprs: /* empty */    
     | expr           
     | exprs ',' expr 
     ;

simpleType: fullID                   
          | ARRAY '[' simpleType ']' 
          ;

blockStats: blockStat                 
          | blockStats semi blockStat 
          ;

blockStat: /* empty */ 
	 | varDefs     
         | expr        
         ;

ids: fullID         
   | ids ',' fullID 
   ;

/* --------------------- FUNC --------------------- */

funcParamClause: nlo '(' ')'            
               | nlo '(' funcParams ')' 
               ;

funcParams: funcParam                
          | funcParams ',' funcParam 
          ;

funcParam: fullID simpleType 
         ;

/* --------------------- FUNC --------------------- */

/* --------------------- CLASS --------------------- */

classParamClause: nlo '(' ')'             
                | nlo '(' classParams ')' 
                ;

classParams: classParam                 
           | classParams ',' classParam 
           ;

classParam: modifiers VAL fullID ':' simpleType 
          | modifiers VAR fullID ':' simpleType 
          ;

modifiers: /* empty */        
         | modifiers modifier 
         ;

modifier: ABSTRACT       
        | FINAL          
        | SEALED         
        | accessModifier 
        | OVERRIDE       
        ;

accessModifier: PRIVATE   
	      | PROTECTED 
	      ;

/* --------------------- CLASS --------------------- */

templateBody: nlo '{' templateStats '}' 
            ;

templateStats: templateStat                     
             | templateStats semi templateStat  
             ;

templateStat: /* empty */   
            | modifiers def 
            | modifiers dcl 
            ;

/* --------------------- DECL --------------------- */

dcl: VAL ids ':' simpleType 
   | VAR ids ':' simpleType 
   | DEF funSig simpleTypeO 
   ;

funSig: fullID funcParamClause 
      ;

/* --------------------- DECL --------------------- */

/* --------------------- DEFS --------------------- */

varDefs: VAL ids simpleTypeO '=' expr 
       | VAR ids simpleTypeO '=' expr 
       ;

def: varDefs    
   | DEF funDef 
   ;

funDef: funSig simpleTypeO '=' expr       
      | THIS funcParamClause '=' constrExpr 
      ;

constrExpr: THIS argumentExprs  // вызов первичного конструктора, бывший selfInvocation
          | '{' THIS argumentExprs semi blockStats '}' 
          | '{' THIS argumentExprs '}' 
          ;

classDef: fullID accessModifier classParamClause classTemplateOpt 
        | fullID classParamClause classTemplateOpt                
        | fullID classTemplateOpt                                 
        ;

classTemplateOpt: /* empty */ %prec LOW_PREC 
                | EXTENDS classTemplate      
                | templateBody               
                ;

classTemplate: classParents templateBody       
             | classParents %prec END_TEMPLATE 
             ;

classParents: fullID argumentExprs  // (бывший constrInvoke)
	    | fullID  // (бывший constrInvoke)
	    ;

topStatSeq: topStat                 
	  | topStatSeq semi topStat 
          ;

topStat: modifiers CLASS classDef 
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
    | NL nlo // не используется nls, тк достаточно опциональности на 1 символ, тк по грамматикам Scala подряд может идит лишь 2 NL(на это есть ограничение в лексере)
    ;

semio: /* empty */
     | semi
     ;

fullID: '+'              
      | '!'              
      | '#'              
      | '%'              
      | '&'              
      | '*'              
      | '-'              
      | '/'              
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
      | GREATER_OR_EQUAL 
      | LESS_OR_EQUAL    
      | ID               
      | ID_EQUALITY      
      | ID_VERTICAL_SIGN 
      | ID_AMPERSAND     
      | ID_CIRCUMFLEX    
      | ID_LESS          
      | ID_GREAT         
      | ID_MINUS         
      | ID_PLUS          
      | ID_ASTERISK      
      | ID_SLASH         
      | ID_PERCENT       
      | ID_EXCLAMATION   
      | ID_TILDE   	 
      | ID_COLON         
      ;

%%

const char* get_bison_token_name(int token) {
            return yytname[YYTRANSLATE(token)];
}