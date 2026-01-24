#ifndef SCALA_LEXER_NAMETRANSFORMER_H
#define SCALA_LEXER_NAMETRANSFORMER_H

#pragma once
#include <string>
#include <unordered_map>
#include <cctype>
#include <sstream>
#include <iomanip>


class NameTransformer {
public:
    static std::string encode(const std::string& name);
    static std::string decode(const std::string& name);
    static std::string toUnicodeEscape(unsigned char c);

private:
    static const std::unordered_map<std::string, char> op;
    static const std::unordered_map<char, std::string> encodeMap;

    static bool isJavaIdent(unsigned char c);

    static bool isHex(const std::string& s);

    static bool isJvmIdentChar(char c);
};


#endif //SCALA_LEXER_NAMETRANSFORMER_H
