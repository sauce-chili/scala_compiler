%option noyywrap never-interactive yylineno
%option prefix="real_"
%x MULTILINE_COMMENT
%x STRING
%x CHARACTER

%{
	#include <iostream>
    #include <string>
    #include <vector>
    #include "lexer/BufferedYYLex.h"
    #include "lexer/TokenProcessor.h"

    /* Объявление оригинальной функции Flex */
    int real_lex();

    BufferedYYLex bufferedLexer{std::function<int()>(real_lex)};
    TokenProcessor processor{bufferedLexer};
%}

digit [0-9]
notZeroDigit [1-9]
binDigit [01]
hexDigit [0-9a-fA-F]

number {digit}("_"*{digit})*
exponent [eE][+-]?{number}
floatSuffix [fFdD]

doubleLiteral {number}\.{number}?{exponent}?{floatSuffix}?|\.{number}{exponent}?{floatSuffix}?|{number}{exponent}{floatSuffix}?|{number}{exponent}?{floatSuffix}
decimalLiteral {number}
hexLiteral 0(x|X)(_*{hexDigit})*
binLiteral 0(b|B)(_*{binDigit})*

UnicodeEscape \\u+{hexDigit}{4}
latinLetter [a-zA-Z]

ID ([a-zA-Z_$](_*[a-zA-Z0-9$]+)*(_[\!\#\%\&\*\+\-\:\/\<\=\>\?\@\\\^\|\~]*)*)|[\!\#\%\&\*\+\-\:\/\<\=\>\?\@\\\^\|\~]*

whitespace [ \t\f\u000B]
%%

%{
    // локальная область
    std::string buff = "";
    std::string literal = "";
%}


"//".* {
         /* skip single line comment */
         //printf("Oneline comment: %s\n", yytext + 2);
}

"/*" { BEGIN(MULTILINE_COMMENT); buff = ""; }
<MULTILINE_COMMENT>[^*]+ { buff += yytext; }
<MULTILINE_COMMENT>\*+[^/] { buff += yytext; }
<MULTILINE_COMMENT>\*+\/ { /* std::cout << "Multiline comment: " << buff << std::endl; */ BEGIN(INITIAL); }

"Int"|"Double"|"String"|"Char"|"Boolean"|"Unit" {
    return processor.onID(yytext);
}

"Array"     { return processor.onToken(ARRAY); }

"class"     { return processor.onToken(CLASS); }
"trait"     { return processor.onToken(TRAIT); }
"object"    { return processor.onToken(OBJECT); }
"enum"      { return processor.onToken(ENUM); }

"abstract"  { return processor.onToken(ABSTRACT); }
"final"     { return processor.onToken(FINAL); }
"sealed"    { return processor.onToken(SEALED); }
"override"  { return processor.onToken(OVERRIDE); }
"private"   { return processor.onToken(PRIVATE); }
"protected" { return processor.onToken(PROTECTED); }

"def"       { return processor.onToken(DEF); }
"val"       { return processor.onToken(VAL); }
"var"       { return processor.onToken(VAR); }

"if"        { return processor.onToken(IF); }
"else"      { return processor.onToken(ELSE); }
"while"     { return processor.onToken(WHILE); }
"do"        { return processor.onToken(DO); }
"for"       { return processor.onToken(FOR); }
"yield"     { return processor.onToken(YIELD); }
"case"      { return processor.onToken(CASE); }
"try"       { return processor.onToken(TRY); }
"catch"     { return processor.onToken(CATCH); }
"finally"   { return processor.onToken(FINALLY); }
"throw"     { return processor.onToken(THROW); }
"return"    { return processor.onToken(RETURN); }

"extends"   { return processor.onToken(EXTENDS); }
"with"      { return processor.onToken(WITH); }

"new"       { return processor.onToken(NEW); }
"super"     { return processor.onToken(SUPER); }
"this"      { return processor.onToken(THIS); }

"true"      { return processor.onBooleanLiteral(TRUE_LITERAL, true); }
"false"     { return processor.onBooleanLiteral(TRUE_LITERAL, false); }
"null"      { return processor.onToken(NULL_LITERAL); }

{decimalLiteral} { return processor.onDecimalLiteral(yytext); }
{hexLiteral}     { return processor.onHexLiteral(yytext); }
{binLiteral}     { return processor.onBinLiteral(yytext); }
{doubleLiteral}  { return processor.onDoubleLiteral(yytext); }

"."     { return processor.onToken('.'); }
","     { return processor.onToken(','); }
":"     { return processor.onToken(':'); }

"("     { return processor.onToken('('); }
")"     { return processor.onToken(')'); }
"["     { return processor.onToken('['); }
"]"     { return processor.onToken(']'); }
"{"     { return processor.onToken('{'); }
"}"     { return processor.onToken('}'); }

"<-"    { return processor.onToken(LEFT_ARROW); }

"+"     { return processor.onToken('+'); }
"-"     { return processor.onToken('-'); }
"*"     { return processor.onToken('*'); }
"/"     { return processor.onToken('/'); }
"%"     { return processor.onToken('%'); }

"=="    { return processor.onToken(EQUAL); }
"!="    { return processor.onToken(NOT_EQUAL); }
">"     { return processor.onToken('>'); }
"<"     { return processor.onToken('<'); }
">="    { return processor.onToken(GREATER_OR_EQUAL); }
"<="    { return processor.onToken(LESS_OR_EQUAL); }

"!"     { return processor.onToken('!'); }
"~"     { return processor.onToken('~'); }

"&"     { return processor.onToken('&'); }
"|"     { return processor.onToken('|'); }
"^"     { return processor.onToken('^'); }

"="     { return processor.onToken('='); }
"+="    { return processor.onToken(PLUS_ASSIGNMENT); }
"-="    { return processor.onToken(MINUS_ASSIGNMENT); }
"*="    { return processor.onToken(MUL_ASSIGNMENT); }
"/="    { return processor.onToken(DIV_ASSIGNMENT); }
"%="    { return processor.onToken(MOD_ASSIGNMENT); }

{ID} { return processor.onID(yytext); }

\"                     { literal = ""; BEGIN(STRING); }
<STRING>[^\\\n\"]+     { literal.append(yytext); };
<STRING,CHARACTER>\\n  { literal.append("\n"); }
<STRING,CHARACTER>\\\\ { literal.append("\\"); }
<STRING,CHARACTER>\\b  { literal.append("\b"); }
<STRING,CHARACTER>\\t  { literal.append("\t"); }
<STRING,CHARACTER>\\f  { literal.append("\f"); }
<STRING,CHARACTER>\\r  { literal.append("\r"); }
<STRING,CHARACTER>\\\" { literal.append("\""); }
<STRING,CHARACTER>\\\' { literal.append("\'"); }
<STRING,CHARACTER>\\. {
    std::cerr << "Unknown escape sequence: " << yytext << "\n" << std::endl;
    BEGIN(INITIAL);
}
<STRING,CHARACTER>{UnicodeEscape} {
    char ch = (char)strtol(yytext + strlen(yytext) - 4, NULL, 16);
    literal.append(1, ch);
}
<STRING>\" {
    BEGIN(INITIAL);
    return processor.onStringLiteral(literal);
}
<STRING,CHARACTER>\n {
    std::cerr << "error: newline inside literal\n" << std::endl;
    BEGIN(INITIAL);
}
<STRING><<EOF>> {
    std::cerr << "error: file ends before literal ends\n" << std::endl;
    BEGIN(INITIAL);
}

\' { literal = ""; BEGIN(CHARACTER); }
<CHARACTER>[^\'\\\n]+ { literal.append(yytext); }
<CHARACTER>\' {
    if(literal.length() == 0){
        std::cerr << "error: empty character\n" << std::endl;
    } else if (literal.length() == 1){
        return processor.onChar(literal[0]);
    } else {
        std::cerr << "\nerror: more than one character in string literal: " << literal << std::endl;
    }
    BEGIN(INITIAL);
}
<CHARACTER><<EOF>> {
    std::cerr << "error: no end of character\n" << std::endl;
    BEGIN(INITIAL);
}

\n {
    processor.onNewLine();
    /* Ничего не возвращаем парсеру здесь, Flex крутится дальше */
}

";"     { processor.onSemicolon(); }

<*>" "   { }
<*>.     { std::cerr << "Unexpected char \'" << yytext[0] << "\'\n"; BEGIN(INITIAL);}

%%

/* Снимаем макрос, который Flex мог наложить */
#ifdef yylex
#undef yylex
#endif

int yylex() {
    return bufferedLexer.getNextToken(); // Название метода, которое мы сменили
}