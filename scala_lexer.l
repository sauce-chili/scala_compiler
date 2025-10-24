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
    std::string peek_next_word();
    int peek_nonspace_char();

    /* --- State variables --- */
    int parenDepth = 0;
    int bracketDepth = 0;
    int braceDepth = 0;
    bool last_token_can_end_stmt = false;
    std::stack<int> case_stack;

    #define NL 9999
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

ID [a-zA-Z_$]+[a-zA-Z0-9_$]*

whitespace [ \t\f\u000B]
%%

%{
    std::string buff = "";
    std::string literal = "";
%}

"//".* { printf("Oneline comment: %s\n", yytext + 2); }

"/*" { BEGIN(MULTILINE_COMMENT); buff = ""; }
<MULTILINE_COMMENT>[^*]+ { buff += yytext; }
<MULTILINE_COMMENT>\*+[^/] { buff += yytext; }
<MULTILINE_COMMENT>\*+\/ { std::cout << "Multiline comment: " << buff << std::endl; BEGIN(INITIAL); }


Int     { printf("Integer type: %s\n", yytext); }
Char    { printf("Character type: %s\n", yytext); }
String  { printf("String type: %s\n", yytext); }
Boolean { printf("Boolean type: %s\n", yytext); }
Unit    { printf("Unit type: %s\n", yytext); }
Array   { printf("Array type: %s\n", yytext); }
Double  { printf("Double type: %s\n", yytext); }


class       { printTypeDefinitionKeyword(yytext); last_token_can_end_stmt = false;}
trait       { printTypeDefinitionKeyword(yytext); last_token_can_end_stmt = false;}
object      { printTypeDefinitionKeyword(yytext); last_token_can_end_stmt = false;}
enum        { printTypeDefinitionKeyword(yytext); last_token_can_end_stmt = false;}
given       { printTypeDefinitionKeyword(yytext); last_token_can_end_stmt = false;}
type        { printTypeDefinitionKeyword(yytext); last_token_can_end_stmt = false;}
opaque      { printTypeDefinitionKeyword(yytext); last_token_can_end_stmt = false;}
open        { printTypeDefinitionKeyword(yytext); last_token_can_end_stmt = false;}
extension   { printTypeDefinitionKeyword(yytext); last_token_can_end_stmt = false;}

abstract    { printModificatorKeyword(yytext); }
final       { printModificatorKeyword(yytext); }
sealed      { printModificatorKeyword(yytext); }
override    { printModificatorKeyword(yytext); }
implicit    { printModificatorKeyword(yytext); }
inline      { printModificatorKeyword(yytext); }
lazy        { printModificatorKeyword(yytext); }
transparent { printModificatorKeyword(yytext); }
private     { printModificatorKeyword(yytext); }
protected   { printModificatorKeyword(yytext); }
derives     { printModificatorKeyword(yytext); }

def         { printMembersDeclarationKeyword(yytext); last_token_can_end_stmt = false;}
val         { printMembersDeclarationKeyword(yytext); last_token_can_end_stmt = false;}
var         { printMembersDeclarationKeyword(yytext); last_token_can_end_stmt = false;}
package     { printMembersDeclarationKeyword(yytext); last_token_can_end_stmt = false;}
import      { printMembersDeclarationKeyword(yytext); last_token_can_end_stmt = false;}
export      { printMembersDeclarationKeyword(yytext); last_token_can_end_stmt = false;}
as          { printMembersDeclarationKeyword(yytext); last_token_can_end_stmt = false;}
using       { printMembersDeclarationKeyword(yytext); last_token_can_end_stmt = false;}

if          { printControlStructureKeyword(yytext); last_token_can_end_stmt = false;}
then        { printControlStructureKeyword(yytext); last_token_can_end_stmt = false;}
else        { printControlStructureKeyword(yytext); last_token_can_end_stmt = false;}
while       { printControlStructureKeyword(yytext); last_token_can_end_stmt = false;}
do          { printControlStructureKeyword(yytext); last_token_can_end_stmt = false;}
for         { printControlStructureKeyword(yytext); last_token_can_end_stmt = false;}
yield       { printControlStructureKeyword(yytext); last_token_can_end_stmt = false;}
match       { printControlStructureKeyword(yytext); last_token_can_end_stmt = false;}
case        { printControlStructureKeyword(yytext); case_stack.push(1); last_token_can_end_stmt = false;}
try         { printControlStructureKeyword(yytext); last_token_can_end_stmt = false;}
catch       { printControlStructureKeyword(yytext); last_token_can_end_stmt = false;}
finally     { printControlStructureKeyword(yytext); last_token_can_end_stmt = false;}
throw       { printControlStructureKeyword(yytext); last_token_can_end_stmt = false;}
return      { printControlStructureKeyword(yytext); last_token_can_end_stmt = true;}

extends     { printInheritKeyword(yytext); last_token_can_end_stmt = false;}
with        { printInheritKeyword(yytext); last_token_can_end_stmt = false;}

new         { printObjectDefinitionKeyword(yytext); last_token_can_end_stmt = false;}
super       { printObjectDefinitionKeyword(yytext); last_token_can_end_stmt = false;}
this        { printObjectDefinitionKeyword(yytext); last_token_can_end_stmt = true;}

end         { printContextKeyword(yytext); last_token_can_end_stmt = false;}
infix       { printContextKeyword(yytext); last_token_can_end_stmt = false;}

true        { printConstantKeyword(yytext); last_token_can_end_stmt = true;}
false       { printConstantKeyword(yytext); last_token_can_end_stmt = true;}
null        { printConstantKeyword(yytext); last_token_can_end_stmt = true;}


{decimalLiteral} {
    long long val = to_int(yytext, 10);
    std::cout << "Decimal literal: " << yytext << " -> " << val << std::endl;
    last_token_can_end_stmt = true;
}

{hexLiteral} {
    long long val = to_int(yytext, 16);
    std::cout << "Hex literal: " << yytext << " -> " << val << std::endl;
    last_token_can_end_stmt = true;
}

{binLiteral} {
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
    double val = to_double(yytext);
    std::cout << "Double literal: " << yytext << " -> " << val << std::endl;
    last_token_can_end_stmt = true;
}


{ID} { std::cout << "Identifier: " << yytext << std::endl; last_token_can_end_stmt = true;}

"."     { printSyntaxToken(yytext); last_token_can_end_stmt = false;}

","     { printSyntaxToken(yytext); last_token_can_end_stmt = false;}

":"     { printSyntaxToken(yytext); last_token_can_end_stmt = false;}

";"     { printSyntaxToken(yytext); last_token_can_end_stmt = true;}

"("     { parenDepth++; last_token_can_end_stmt = false; printOperator(yytext); }
")"     { if(parenDepth>0) parenDepth--; last_token_can_end_stmt = true; printOperator(yytext); }

"["     { bracketDepth++; last_token_can_end_stmt = false; printOperator(yytext); }
"]"     { if(bracketDepth>0) bracketDepth--; last_token_can_end_stmt = true; printOperator(yytext); }

"{"     { braceDepth++; last_token_can_end_stmt = false; printOperator(yytext); }
"}"     { if(braceDepth>0) braceDepth--; last_token_can_end_stmt = true; printOperator(yytext); }


"=>"    { if (!case_stack.empty()) case_stack.pop(); last_token_can_end_stmt = false; printOperator(yytext); }
"<-"    { last_token_can_end_stmt = false; printOperator(yytext); }

"+"     { printOperator(yytext); last_token_can_end_stmt = false;}
"-"     { printOperator(yytext); last_token_can_end_stmt = false;}
"*"     { printOperator(yytext); last_token_can_end_stmt = false;}
"/"     { printOperator(yytext); last_token_can_end_stmt = false;}
"%"     { printOperator(yytext); last_token_can_end_stmt = false;}

"=="    { printOperator(yytext); last_token_can_end_stmt = false;}
"!="    { printOperator(yytext); last_token_can_end_stmt = false;}
">"     { printOperator(yytext); last_token_can_end_stmt = false;}
"<"     { printOperator(yytext); last_token_can_end_stmt = false;}
">="    { printOperator(yytext); last_token_can_end_stmt = false;}
"<="    { printOperator(yytext); last_token_can_end_stmt = false;}

"&&"    { printOperator(yytext); last_token_can_end_stmt = false;}
"||"    { printOperator(yytext); last_token_can_end_stmt = false;}
"!"     { printOperator(yytext); last_token_can_end_stmt = false;}

"&"     { printOperator(yytext); last_token_can_end_stmt = false;}
"|"     { printOperator(yytext); last_token_can_end_stmt = false;}
"^"     { printOperator(yytext); last_token_can_end_stmt = false;}
"~"     { printOperator(yytext); last_token_can_end_stmt = false;}
"<<"    { printOperator(yytext); last_token_can_end_stmt = false;}
">>"    { printOperator(yytext); last_token_can_end_stmt = false;}
">>>"   { printOperator(yytext); last_token_can_end_stmt = false;}

"="     { printOperator(yytext); last_token_can_end_stmt = false;}
"+="    { printOperator(yytext); last_token_can_end_stmt = false;}
"-="    { printOperator(yytext); last_token_can_end_stmt = false;}
"*="    { printOperator(yytext); last_token_can_end_stmt = false;}
"/="    { printOperator(yytext); last_token_can_end_stmt = false;}
"%="    { printOperator(yytext); last_token_can_end_stmt = false;}
"<<="   { printOperator(yytext); last_token_can_end_stmt = false;}
">>="   { printOperator(yytext); last_token_can_end_stmt = false;}
"&="    { printOperator(yytext); last_token_can_end_stmt = false;}
"^="    { printOperator(yytext); last_token_can_end_stmt = false;}
"|="    { printOperator(yytext); last_token_can_end_stmt = false;}

"to"    { printOperator(yytext); last_token_can_end_stmt = false;}
"until" { printOperator(yytext); last_token_can_end_stmt = false;}


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
    if(literal.length() == 0){
        std::cerr << "error: empty character\n" << std::endl;
    } else if (literal.length() == 1){
        char ch = literal[0];
        printf("Char literal: %c\n", ch);
    } else {
        std::cerr << "error: more than one character in string literal: " << literal << std::endl;
    }
    last_token_can_end_stmt = true;
    BEGIN(INITIAL);
}
<CHARACTER><<EOF>> {
    std::cerr << "error: no end of character\n" << std::endl;
    BEGIN(INITIAL);
}


{whitespace}  { std::cout << "+++Whitespace+++" << std::endl; }
"\n" {
    int nextch = peek_nonspace_char();
    bool blank_line_after = (nextch == '\n');
    bool cond1 = last_token_can_end_stmt;
    std::string nextWord = peek_next_word();
    bool cond2 = true;
    if (!nextWord.empty()) {
        if ( nextWord == "catch" || nextWord == "else" || nextWord == "extends" || nextWord == "finally" ||
             nextWord == "forSome" || nextWord == "match" || nextWord == "with" || nextWord == "yield" ||
             nextWord == "," || nextWord == "." || nextWord == ";" || nextWord == ":" || nextWord == "=" ||
             nextWord == "=>" || nextWord == "<-" || nextWord == "<:" || nextWord == "<%" || nextWord == ">:" ||
             nextWord == "#" || nextWord == "[" || nextWord == ")" || nextWord == "]" || nextWord == "}" )
            cond2 = false;
        if (nextWord == "case") {
            std::string afterCase = peek_next_word();
            if (!(afterCase == "class" || afterCase == "object")) cond2 = false;
        }
    } else cond2 = false;

    bool newlines_enabled = (parenDepth == 0 && bracketDepth == 0 && case_stack.empty());

    if (cond1 && cond2 && newlines_enabled) {
        std::cout << "NL token emitted" << std::endl;
        last_token_can_end_stmt = false;
        //return NL;
    } else if (blank_line_after && cond1 && newlines_enabled) {
        std::cout << "NL token (blank-line) emitted" << std::endl;
        last_token_can_end_stmt = false;
        //return NL;
    } else {
        /* skip */
    }
}

<*>.     { std::cerr << "Unexpected char \'" << yytext[0] << "\'"; BEGIN(INITIAL);}

%%

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
    yylex();
    fclose(yyin);
    return 0;
}

std::string peek_next_word() {
    std::string word = "";
    int c;
    while ((c = yyinput()) != EOF) {
        if (c == ' ' || c == '\t' || c == '\r') continue;
        if (c == '/') {
            int c2 = yyinput();
            if (c2 == '/') {
                int cc;
                while ((cc = yyinput()) != EOF && cc != '\n') {}
                if (cc == '\n') { unput(cc); continue; }
                return word;
            } else if (c2 == '*') {
                int cc, prev = 0;
                while ((cc = yyinput()) != EOF) {
                    if (prev == '*' && cc == '/') break;
                    prev = cc;
                }
                continue;
            } else {
                unput(c2);
                unput(c);
                break;
            }
        }
        break;
    }
    if (c == EOF) return word;

    if (isalpha(c) || c == '_') {
        word.push_back((char)c);
        while ((c = yyinput()) != EOF && (isalnum(c) || c == '_')) {
            word.push_back((char)c);
        }
        if (c != EOF) unput(c);
        return word;
    } else {
        word.push_back((char)c);
        int c2 = yyinput();
        if (c2 != EOF) {
            std::string two = word + (char)c2;
            if (two == "=>" || two == "<-" || two == "<:" || two == "<%" || two == ">:" )
                word = two;
            else
                unput(c2);
        }
        return word;
    }
}

int peek_nonspace_char() {
    int c;
    while ((c = yyinput()) != EOF) {
        if (c == ' ' || c == '\t' || c == '\r') continue;

        if (c == '/') {
            int c2 = yyinput();
            if (c2 == '/') {
                int cc;
                while ((cc = yyinput()) != EOF && cc != '\n') {}
                if (cc == '\n') { unput(cc); continue; }
            } else if (c2 == '*') {
                int cc, prev = 0;
                while ((cc = yyinput()) != EOF) {
                    if (prev == '*' && cc == '/') break;
                    prev = cc;
                }
                continue;
            } else {
                unput(c2);
                unput(c);
                return c;
            }
        }
        unput(c);
        return c;
    }
    return EOF;
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