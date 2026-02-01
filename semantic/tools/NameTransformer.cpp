#include "NameTransformer.h"
#include "semantic/tables/tables.hpp"

const std::unordered_map<std::string, char> NameTransformer::op = {
        {"$tilde",'~'}, {"$eq",'='}, {"$less",'<'}, {"$greater",'>'},
        {"$bang",'!'},  {"$hash",'#'}, {"$percent",'%'}, {"$up",'^'},
        {"$amp",'&'},   {"$bar",'|'}, {"$times",'*'}, {"$div",'/'},
        {"$plus",'+'},  {"$minus",'-'}, {"$colon",':'},
        {"$bslash",'\\'}, {"$qmark",'?'}, {"$at",'@'}
};

const std::unordered_map<char, std::string> NameTransformer::encodeMap = {
        {'~', "$tilde"}, {'=', "$eq"}, {'<', "$less"}, {'>', "$greater"},
        {'!', "$bang"}, {'#', "$hash"}, {'%', "$percent"}, {'^', "$up"},
        {'&', "$amp"}, {'|', "$bar"}, {'*', "$times"}, {'/', "$div"},
        {'+', "$plus"}, {'-', "$minus"}, {':', "$colon"},
        {'\\', "$bslash"}, {'?', "$qmark"}, {'@', "$at"}
};

std::string NameTransformer::encode(const std::string& name) {
    std::string out;

    if (name == CONSTRUCTOR_NAME) {
        out = JVM_CONSTRUCTOR_NAME;
    } else {
        for (char c: name) {
            auto it = encodeMap.find(c);
            if (it != encodeMap.end()) {
                out += it->second;
            } else if (isJvmIdentChar(c)) {
                out += c;
            } else {
                out += toUnicodeEscape(static_cast<unsigned char>(c));
            }
        }
    }
    return out;
}

std::string NameTransformer::decode(const std::string& name) {
    std::string out;

    if (name == JVM_CONSTRUCTOR_NAME) {
        out = CONSTRUCTOR_NAME;
    } else {
        for (size_t i = 0; i < name.size();) {
            if (name[i] == '$') {
                // operator?
                bool matched = false;
                for (auto &[code, ch]: op) {
                    if (name.compare(i, code.size(), code) == 0) {
                        out += ch;
                        i += code.size();
                        matched = true;
                        break;
                    }
                }
                if (matched) continue;

                // unicode $uXXXX ?
                if (i + 5 < name.size() && name[i + 1] == 'u') {
                    std::string hex = name.substr(i + 2, 4);
                    if (isHex(hex)) {
                        char c = static_cast<char>(std::stoi(hex, nullptr, 16));
                        out += c;
                        i += 6;
                        continue;
                    }
                }
            }
            out += name[i++];
        }
    }
    return out;
}

bool NameTransformer::isJavaIdent(unsigned char c) {
    return std::isalnum(c) || c == '_';
}

bool NameTransformer::isHex(const std::string& s) {
    for (char c: s)
        if (!std::isxdigit(static_cast<unsigned char>(c)))
            return false;
    return true;
}

bool NameTransformer::isJvmIdentChar(char c) {
    return std::isalnum((unsigned char)c) || c == '_' || c == '$';
}

std::string NameTransformer::toUnicodeEscape(unsigned char c) {
    const char hex[] = "0123456789ABCDEF";
    std::string out = "$u0000";
    out[2] = hex[(c >> 4) & 0xF];
    out[3] = hex[c & 0xF];
    return out;
}