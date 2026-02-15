#ifndef SCALA_LEXER_COMMONCONSTANTS_H
#define SCALA_LEXER_COMMONCONSTANTS_H

#include <cstdio>
#include <string>

extern FILE *real_in;
extern int real_lineno;
extern int yydebug;

extern std::string real_path;

extern const std::string CONSTRUCTOR_NAME;
extern const std::string JVM_CONSTRUCTOR_NAME;
extern const std::string BASE_SCALA_CLASS;

#endif //SCALA_LEXER_COMMONCONSTANTS_H
