%option noyywrap never-interactive
%x MULTILINE_COMMENT
%x STRING
%x CHARACTER

%{
	#include <iostream>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <string>
	#include <cstring>
	#include <stdbool.h>
	#include <stack>
	#include <algorithm>
	#include <vector>
	#include <set>

	#define NL 9999

    void printTypeDefinitionKeyword(char* keyword);
    void printModificatorKeyword(char* keyword);
    void printMembersDeclarationKeyword(char* keyword);
    void printControlStructureKeyword(char* keyword);
    void printInheritKeyword(char* keyword);
    void printObjectDefinitionKeyword(char* keyword);
    void printContextKeyword(char* keyword);
    void printConstantKeyword(char* keyword);
    void printOperator(char* oper);
    void printSyntaxToken(char* token);
    std::string remove_underscore(const char* text);
    long long to_int(const char* text, int base = 10);
    double to_double(const char* text);

    bool last_token_can_end_stmt = false;

    std::vector<bool> nl_enabled_stack;

    // pending newlines: 0 (none), 1 (one newline), 2 (blank-line detected -> treated as blank)
    int pending_nl = 0;

    // helper declarations for new newline-handling strategy
    void handle_pending_newlines(const std::string &curTok, bool curTokCanBegin);
    bool cur_token_can_begin_statement(const std::string &tok);
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
    std::string buff = "";
    std::string literal = "";
%}


"//".* {
         /* print optionally for debug:
            printf("Oneline comment: %s\n", yytext + 2);
         */
       }

"/*" { BEGIN(MULTILINE_COMMENT); buff = ""; }
<MULTILINE_COMMENT>[^*]+ { buff += yytext; }
<MULTILINE_COMMENT>\*+[^/] { buff += yytext; }
<MULTILINE_COMMENT>\*+\/ { /* std::cout << "Multiline comment: " << buff << std::endl; */ BEGIN(INITIAL); }

Int     { printf("Integer type: %s\n",   yytext); }
Char    { printf("Character type: %s\n", yytext); }
String  { printf("String type: %s\n",    yytext); }
Boolean { printf("Boolean type: %s\n",   yytext); }
Unit    { printf("Unit type: %s\n",      yytext); }
Array   { printf("Array type: %s\n",     yytext); }
Double  { printf("Double type: %s\n",    yytext); }

class       { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printTypeDefinitionKeyword(yytext); last_token_can_end_stmt = false; }
trait       { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printTypeDefinitionKeyword(yytext); last_token_can_end_stmt = false; }
object      { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printTypeDefinitionKeyword(yytext); last_token_can_end_stmt = false; }
enum        { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printTypeDefinitionKeyword(yytext); last_token_can_end_stmt = false; }
given       { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printTypeDefinitionKeyword(yytext); last_token_can_end_stmt = false; }
type        { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printTypeDefinitionKeyword(yytext); last_token_can_end_stmt = false; }
opaque      { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printTypeDefinitionKeyword(yytext); last_token_can_end_stmt = false; }
open        { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printTypeDefinitionKeyword(yytext); last_token_can_end_stmt = false; }
extension   { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printTypeDefinitionKeyword(yytext); last_token_can_end_stmt = false; }

abstract    { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printModificatorKeyword(yytext); }
final       { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printModificatorKeyword(yytext); }
sealed      { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printModificatorKeyword(yytext); }
override    { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printModificatorKeyword(yytext); }
implicit    { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printModificatorKeyword(yytext); }
inline      { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printModificatorKeyword(yytext); }
lazy        { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printModificatorKeyword(yytext); }
transparent { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printModificatorKeyword(yytext); }
private     { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printModificatorKeyword(yytext); }
protected   { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printModificatorKeyword(yytext); }
derives     { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printModificatorKeyword(yytext); }

def         { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printMembersDeclarationKeyword(yytext); last_token_can_end_stmt = false;}
val         { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printMembersDeclarationKeyword(yytext); last_token_can_end_stmt = false;}
var         { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printMembersDeclarationKeyword(yytext); last_token_can_end_stmt = false;}
package     { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printMembersDeclarationKeyword(yytext); last_token_can_end_stmt = false;}
import      { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printMembersDeclarationKeyword(yytext); last_token_can_end_stmt = false;}
export      { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printMembersDeclarationKeyword(yytext); last_token_can_end_stmt = false;}
as          { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printMembersDeclarationKeyword(yytext); last_token_can_end_stmt = false;}
using       { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printMembersDeclarationKeyword(yytext); last_token_can_end_stmt = false;}

if          { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printControlStructureKeyword(yytext); last_token_can_end_stmt = false;}
then        { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printControlStructureKeyword(yytext); last_token_can_end_stmt = false;}
else        { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printControlStructureKeyword(yytext); last_token_can_end_stmt = false;}
while       { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printControlStructureKeyword(yytext); last_token_can_end_stmt = false;}
do          { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printControlStructureKeyword(yytext); last_token_can_end_stmt = false;}
for         { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printControlStructureKeyword(yytext); last_token_can_end_stmt = false;}
yield       { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printControlStructureKeyword(yytext); last_token_can_end_stmt = false;}
case        { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printControlStructureKeyword(yytext); last_token_can_end_stmt = false;}
try         { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printControlStructureKeyword(yytext); last_token_can_end_stmt = false;}
catch       { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printControlStructureKeyword(yytext); last_token_can_end_stmt = false;}
finally     { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printControlStructureKeyword(yytext); last_token_can_end_stmt = false;}
throw       { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printControlStructureKeyword(yytext); last_token_can_end_stmt = false;}
return      { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printControlStructureKeyword(yytext); last_token_can_end_stmt = true;}

extends     { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printInheritKeyword(yytext); last_token_can_end_stmt = false;}
with        { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printInheritKeyword(yytext); last_token_can_end_stmt = false;}

new         { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printObjectDefinitionKeyword(yytext); last_token_can_end_stmt = false;}
super       { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printObjectDefinitionKeyword(yytext); last_token_can_end_stmt = false;}
this        { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printObjectDefinitionKeyword(yytext); last_token_can_end_stmt = true;}

end         { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printContextKeyword(yytext); last_token_can_end_stmt = false;}
infix       { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printContextKeyword(yytext); last_token_can_end_stmt = false;}

true        { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printConstantKeyword(yytext); last_token_can_end_stmt = true;}
false       { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printConstantKeyword(yytext); last_token_can_end_stmt = true;}
null        { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printConstantKeyword(yytext); last_token_can_end_stmt = true;}

{decimalLiteral} {
    std::string cur(yytext);
    handle_pending_newlines(cur, cur_token_can_begin_statement(cur));
    long long val = to_int(yytext, 10);
    std::cout << "Decimal literal: " << yytext << " -> " << val << std::endl;
    last_token_can_end_stmt = true;
}

{hexLiteral} {
    std::string cur(yytext);
    handle_pending_newlines(cur, cur_token_can_begin_statement(cur));
    long long val = to_int(yytext, 16);
    std::cout << "Hex literal: " << yytext << " -> " << val << std::endl;
    last_token_can_end_stmt = true;
}

{binLiteral} {
    std::string cur(yytext);
    handle_pending_newlines(cur, cur_token_can_begin_statement(cur));
    std::string s = remove_underscore(yytext);
    // убираем префикс 0b/0B
    if (s.rfind("0b", 0) == 0 || s.rfind("0B", 0) == 0)
        s = s.substr(2);
    long long val = 0;
    for (char c : s) {
        val = (val << 1) | (c - '0');
    }
    std::cout << "Binary literal: " << yytext << " -> " << val << std::endl;
    last_token_can_end_stmt = true;
}

{doubleLiteral} {
    std::string cur(yytext);
    handle_pending_newlines(cur, cur_token_can_begin_statement(cur));
    double val = to_double(yytext);
    std::cout << "Double literal: " << yytext << " -> " << val << std::endl;
    last_token_can_end_stmt = true;
}

"."     { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printSyntaxToken(yytext); last_token_can_end_stmt = false; }
","     { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printSyntaxToken(yytext); last_token_can_end_stmt = false; }
":"     { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printSyntaxToken(yytext); last_token_can_end_stmt = false; }
";"     { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printSyntaxToken(yytext); last_token_can_end_stmt = true; }

"("     {
            std::string cur(yytext);
            handle_pending_newlines(cur, cur_token_can_begin_statement(cur));
            // enter a region where newlines are disabled
            nl_enabled_stack.push_back(false);
            last_token_can_end_stmt = false;
            printOperator(yytext);
        }
")"     {
            std::string cur(yytext);
            handle_pending_newlines(cur, cur_token_can_begin_statement(cur));
            // exit region: pop one frame if present
            if (!nl_enabled_stack.empty()) nl_enabled_stack.pop_back();
            last_token_can_end_stmt = true;
            printOperator(yytext);
        }

"["     {
            std::string cur(yytext);
            handle_pending_newlines(cur, cur_token_can_begin_statement(cur));
            // disable newline in brackets
            nl_enabled_stack.push_back(false);
            last_token_can_end_stmt = false;
            printOperator(yytext);
        }
"]"     {
            std::string cur(yytext);
            handle_pending_newlines(cur, cur_token_can_begin_statement(cur));
            if (!nl_enabled_stack.empty()) nl_enabled_stack.pop_back();
            last_token_can_end_stmt = true;
            printOperator(yytext);
        }

"{"     {
            std::string cur(yytext);
            handle_pending_newlines(cur, cur_token_can_begin_statement(cur));
            // braces always enable newlines even inside disabled region
            nl_enabled_stack.push_back(true);
            last_token_can_end_stmt = false;
            printOperator(yytext);
        }
"}"     {
            std::string cur(yytext);
            handle_pending_newlines(cur, cur_token_can_begin_statement(cur));
            if (!nl_enabled_stack.empty()) nl_enabled_stack.pop_back();
            last_token_can_end_stmt = true;
            printOperator(yytext);
        }

"<-"    { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); last_token_can_end_stmt = false; printOperator(yytext); }

"+"     { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printOperator(yytext); last_token_can_end_stmt = false;}
"-"     { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printOperator(yytext); last_token_can_end_stmt = false;}
"*"     { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printOperator(yytext); last_token_can_end_stmt = false;}
"/"     { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printOperator(yytext); last_token_can_end_stmt = false;}
"%"     { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printOperator(yytext); last_token_can_end_stmt = false;}

"=="    { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printOperator(yytext); last_token_can_end_stmt = false;}
"!="    { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printOperator(yytext); last_token_can_end_stmt = false;}
">"     { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printOperator(yytext); last_token_can_end_stmt = false;}
"<"     { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printOperator(yytext); last_token_can_end_stmt = false;}
">="    { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printOperator(yytext); last_token_can_end_stmt = false;}
"<="    { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printOperator(yytext); last_token_can_end_stmt = false;}

"&&"    { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printOperator(yytext); last_token_can_end_stmt = false;}
"||"    { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printOperator(yytext); last_token_can_end_stmt = false;}
"!"     { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printOperator(yytext); last_token_can_end_stmt = false;}

"&"     { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printOperator(yytext); last_token_can_end_stmt = false;}
"|"     { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printOperator(yytext); last_token_can_end_stmt = false;}
"^"     { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printOperator(yytext); last_token_can_end_stmt = false;}
"~"     { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printOperator(yytext); last_token_can_end_stmt = false;}
"<<"    { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printOperator(yytext); last_token_can_end_stmt = false;}
">>"    { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printOperator(yytext); last_token_can_end_stmt = false;}
">>>"   { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printOperator(yytext); last_token_can_end_stmt = false;}

"="     { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printOperator(yytext); last_token_can_end_stmt = false;}
"+="    { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printOperator(yytext); last_token_can_end_stmt = false;}
"-="    { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printOperator(yytext); last_token_can_end_stmt = false;}
"*="    { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printOperator(yytext); last_token_can_end_stmt = false;}
"/="    { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printOperator(yytext); last_token_can_end_stmt = false;}
"%="    { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printOperator(yytext); last_token_can_end_stmt = false;}
"<<="   { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printOperator(yytext); last_token_can_end_stmt = false;}
">>="   { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printOperator(yytext); last_token_can_end_stmt = false;}
"&="    { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printOperator(yytext); last_token_can_end_stmt = false;}
"^="    { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printOperator(yytext); last_token_can_end_stmt = false;}
"|="    { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printOperator(yytext); last_token_can_end_stmt = false;}

"to"    { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printOperator(yytext); last_token_can_end_stmt = false;}
"until" { std::string cur(yytext); handle_pending_newlines(cur, cur_token_can_begin_statement(cur)); printOperator(yytext); last_token_can_end_stmt = false;}

{ID} {
    std::string cur(yytext);
    handle_pending_newlines(cur, cur_token_can_begin_statement(cur));
    last_token_can_end_stmt = true;
    if (yytext[0] == '*') {
        std::cout << "Asterisk-start identifier: " << yytext << std::endl;
    } else if (yytext[0] == '/') {
        std::cout << "Slash-start identifier: " << yytext << std::endl;
    } else if (yytext[0] == '%') {
        std::cout << "Percent-start identifier: " << yytext << std::endl;
    } else if (yytext[0] == '+') {
        std::cout << "Plus-start identifier: " << yytext << std::endl;
    } else if (yytext[0] == '-') {
       std::cout << "Minus-start identifier: " << yytext << std::endl;
    } else if (yytext[0] == ':') {
       std::cout << "Colon-start identifier: " << yytext << std::endl;
    } else if (yytext[0] == '<') {
       std::cout << "Left-arrow-start identifier: " << yytext << std::endl;
    } else if (yytext[0] == '>') {
       std::cout << "Right-arrow-start identifier: " << yytext << std::endl;
    } else if (yytext[0] == '=') {
       std::cout << "Equality-start identifier: " << yytext << std::endl;
    } else if (yytext[0] == '!') {
       std::cout << "Exclamation-start identifier: " << yytext << std::endl;
    } else if (yytext[0] == '&') {
       std::cout << "Ampersand-start identifier: " << yytext << std::endl;
    } else if (yytext[0] == '^') {
       std::cout << "Upper-arrow-start identifier: " << yytext << std::endl;
    } else if (yytext[0] == '|') {
       std::cout << "Vertical-sign-start identifier: " << yytext << std::endl;
    } else {
       std::cout << "Identifier: " << yytext << std::endl;
    }
}

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
    std::string cur(literal);
    handle_pending_newlines(cur, cur_token_can_begin_statement(cur));
    std::cout << "String literal: " << literal << "\n" << std::endl;
    last_token_can_end_stmt = true;
    BEGIN(INITIAL);
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
    std::string cur = literal;
    handle_pending_newlines(cur, cur_token_can_begin_statement(cur));
    if(literal.length() == 0){
        std::cerr << "error: empty character\n" << std::endl;
    } else if (literal.length() == 1){
        char ch = literal[0];
        printf("Char literal: %c\n", ch);
    } else {
        std::cerr << "\nerror: more than one character in string literal: " << literal << std::endl;
    }
    last_token_can_end_stmt = true;
    BEGIN(INITIAL);
}
<CHARACTER><<EOF>> {
    std::cerr << "error: no end of character\n" << std::endl;
    BEGIN(INITIAL);
}

"\n" {
    int count = 1;
    int c;
    // count consecutive newlines
    while ((c = yyinput()) != EOF) {
        if (c == '\n') {
            count++;
            continue;
        }
        // other chars: push back the last read non-newline and stop
        unput(c);
        break;
    }
    // if at least one blank line (two or more consecutive '\n') treat as blank-line type
    pending_nl = (count >= 2) ? 2 : 1;
    /* do not emit now; it will be handled by handle_pending_newlines when the next token arrives */
}

<*>";"   { std::cout << "Semicolon\n";}
<*>" "   { std::cout << "Space \n";}
<*>.     { std::cerr << "Unexpected char \'" << yytext[0] << "\'\n"; BEGIN(INITIAL);}

%%

bool cur_token_can_begin_statement(const std::string &tok) {
    if (tok.empty()) return false;

    static const std::set<std::string> cannotBegin = {
        "catch",
        "else",
        "extends",
        "finally",
        "forSome",
        "with",
        "yield",
        ",",
        ".",
        ";",
        ":",
        "=",
        "<-",
        "<:",
        "<%",
        ">:",
        "#",
        "[",
        ")",
        "]",
        "}",
    };

    if (cannotBegin.count(tok)) return false;
    return true;
}

void handle_pending_newlines(const std::string &curTok, bool curTokCanBegin) {
    if (pending_nl == 0) return;
    bool newlines_enabled = nl_enabled_stack.empty() ? true : nl_enabled_stack.back();
    //std::cout << curTok << ":" << std::endl;
    //std::cout << "last_token_can_end_stmt" << "curTokCanBegin" << "newlines_enabled" << std::endl;
    //std::cout << last_token_can_end_stmt << curTokCanBegin << newlines_enabled << std::endl;
    if (last_token_can_end_stmt && curTokCanBegin && newlines_enabled) {
        if (pending_nl >= 2) {
            std::cout << "NL token (blank-line) emitted" << std::endl;
        } else {
            std::cout << "NL token emitted" << std::endl;
        }
        // after consuming newline as separator, previous token is not considered "can end" any more
        last_token_can_end_stmt = false;
    }
    // consume pending newlines in any suit
    pending_nl = 0;
}

long long to_int(const char* text, int base) {
    std::string s = remove_underscore(text);
    return strtoll(s.c_str(), NULL, base);
}

std::string remove_underscore(const char* text) {
    std::string s(text);
    s.erase(std::remove(s.begin(), s.end(), '_'), s.end());
    return s;
}

double to_double(const char* text) {
    std::string s = remove_underscore(text);
    return atof(s.c_str());
}

void printTypeDefinitionKeyword(char* keyword) {
    printf("Type definition keyword: %s\n", keyword);
}

void printModificatorKeyword(char* keyword) {
    printf("Modificator definition keyword: %s\n", keyword);
}

void printMembersDeclarationKeyword(char* keyword) {
    printf("Member declaration keyword: %s\n", keyword);
}

void printControlStructureKeyword(char* keyword) {
    printf("Control structure keyword: %s\n", keyword);
}

void printInheritKeyword(char* keyword) {
    printf("Inherit keyword: %s\n", keyword);
}

void printObjectDefinitionKeyword(char* keyword) {
    printf("Object definition keyword: %s\n", keyword);
}

void printContextKeyword(char* keyword) {
    printf("Context keyword: %s\n", keyword);
}

void printConstantKeyword(char* keyword) {
    printf("Constant keyword: %s\n", keyword);
}

void printOperator(char* oper) {
    printf("Operator: %s\n", oper);
}

void printSyntaxToken(char* token) {
    printf("Syntax token: %s\n", token);
}

int main(int argc, char **argv)
{
    if(argc < 2)
    {
        printf("\nNot enough arguments. Please specify filename.\n");
        return -1;
    }
    if((yyin = fopen(argv[1], "r")) == NULL)
    {
        printf("\nCannot open file %s.\n", argv[1]);
        return -1;
    }
    yyin = fopen(argv[1], "r");
    // initially newlines are enabled
    nl_enabled_stack.clear();
    nl_enabled_stack.push_back(true);
    pending_nl = 0;
    last_token_can_end_stmt = false;
    yylex();
    fclose(yyin);
    return 0;
}