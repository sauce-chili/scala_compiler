//
// Created by Dmitry Smirnov on 05.01.2026.
//

#include "TokenProcessor.h"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <set>

#include "nodes/Types.h"


TokenProcessor::TokenProcessor(BufferedYYLex &lexer) : bufferedLexer(lexer) {
    nl_enabled_stack.push_back(true); // Изначально переводы строк разрешены
}

std::string TokenProcessor::remove_underscore(const char *text) {
    std::string s(text);
    s.erase(std::ranges::remove(s, '_').begin(), s.end());
    return s;
}

long long TokenProcessor::to_int(const char *text, int base) {
    std::string s = remove_underscore(text);
    if (base == 2) {
        if (s.rfind("0b", 0) == 0 || s.rfind("0B", 0) == 0) s = s.substr(2);
        long long val = 0;
        for (char c: s) val = (val << 1) | (c - '0');
        return val;
    }
    return strtoll(s.c_str(), nullptr, base);
}

double TokenProcessor::to_double(const char *text) {
    std::string s = remove_underscore(text);
    return atof(s.c_str());
}

bool TokenProcessor::can_begin_stmt(const int token)
{
    switch (token)
    {
        // каких-то токенов нет(на пример match или forSome, но мы их и не поддерживаем)
        case CATCH:
        case ELSE:
        case EXTENDS:
        case FINALLY:
        case WITH:
        case YIELD:
        case ',':
        case '.':
        case ';':
        case ':':
        case '=':
        case LEFT_ARROW:
        case '[':
        case ')':
        case ']':
        case '}':
            return false;
        default:
            return true;
    }
}


bool TokenProcessor::can_end_stmt(const int token) {
    switch (token) {
        // каких-то токенов нет(на пример type, но мы их и не поддерживаем)
        case yytokentype::THIS:
        case yytokentype::NULL_LITERAL:
        case TRUE_LITERAL:
        case FALSE_LITERAL:
        case RETURN:
        case '_':
        case ')':
        case ']':
        case '}':
        // вариации id:
        case yytokentype::ID:
        case '+':
        case '!':
        case '#':
        case '%':
        case '&':
        case '*':
        case '-':
        case '/':
        case '<':
        case '>':
        case '?':
        case '@':
        case '\\':
        case '^':
        case '~':
        // ----------- мб удалить этот блоке ----------
        case PLUS_ASSIGNMENT:
        case MINUS_ASSIGNMENT:
        case MUL_ASSIGNMENT:
        case DIV_ASSIGNMENT:
        case MOD_ASSIGNMENT:
        // ------------------------------------------
        case EQUAL:
        case NOT_EQUAL:
        case GREATER_OR_EQUAL:
        case LESS_OR_EQUAL:
        case ID_EQUALITY:
        case ID_VERTICAL_SIGN:
        case ID_AMPERSAND:
        case ID_CIRCUMFLEX:
        case ID_LESS:
        case ID_GREAT:
        case ID_MINUS:
        case ID_PLUS:
        case ID_ASTERISK:
        case ID_SLASH:
        case ID_PERCENT:
        case ID_EXCLAMATION:
        case ID_COLON:
        // Вариации литералов
        case DECIMAL_LITERAL:
        case yytokentype::DOUBLE_LITERAL:
        case yytokentype::STRING_LITERAL:
        case yytokentype::CHAR_LITERAL:
            return true;
        default: return false;
    }
}

void TokenProcessor::update_state_by_current_token(int token) {
    // Обновляем стек скобок (nl zone enable)
    if (token == '(' || token == '[') nl_enabled_stack.push_back(false);
    else if (token == '{') nl_enabled_stack.push_back(true);
    else if ((token == ')' || token == ']') && !nl_enabled_stack.empty()) nl_enabled_stack.pop_back();
    else if (token == '}' && !nl_enabled_stack.empty()) nl_enabled_stack.pop_back();

    // Обновляем флаг окончания выражения
    last_token_can_end_stmt = can_end_stmt(token);
}

void TokenProcessor::onNewLine() {
    if (pending_nl_count < 2) {
        pending_nl_count++;
    }
    // Ничего не возвращаем, лексер Flex просто продолжит работу
}

int TokenProcessor::onToken(int tokenType) {
    // https://scala-lang.org/files/archive/spec/2.13/01-lexical-syntax.html#newline-characters

    // 1. Проверяем условия вставки NL
    bool nl_enabled = nl_enabled_stack.empty() ? true : nl_enabled_stack.back();
    bool should_insert = pending_nl_count > 0 &&
                         nl_enabled &&
                         last_token_can_end_stmt &&
                         can_begin_stmt(tokenType);

    std::cout << "";

    if (should_insert) {
        // Сохраняем текущее значение, тк оно уйдет в буфер
        YYSTYPE currentVal = yylval;
        // Если передан текст и он еще не в yylval (для ID и т.д.), обрабатываем его снаружи,
        // но здесь мы предполагаем, что yylval уже заполнен методами типа onID/onLiteral
        // или самим flex-правилом перед вызовом.

        // int currentLine = yylineno;
        YYSTYPE emptyVal; // Значение для токена NL (пустое)

        if (pending_nl_count == 1) {
            // Логика: Возвращаем NL сейчас. Следующий вызов должен вернуть CurrentToken.
            // Так как BufferedYYLex - это очередь (FIFO), кладем CurrentToken.

            bufferedLexer.push(tokenType, currentVal, -1);

            update_state_by_current_token(tokenType);
            pending_nl_count = 0;
            return NL;
        } else if (pending_nl_count == 2) {
            // pending_nl_count == 2
            // Логика: Возвращаем NL сейчас. Следующий вызов - NL. Третий - CurrentToken.
            // Далее:
            // 1. Кладем NL (чтобы он вышел первым из буфера)
            // 2. Кладем currentToken (чтобы он вышел вторым из буфера)

            bufferedLexer.push(NL, emptyVal, -1);
            bufferedLexer.push(tokenType, currentVal, -1);

            update_state_by_current_token(tokenType);
            pending_nl_count = 0;
            return NL;
        }
    }

    // Иначе: NL не нужен
    update_state_by_current_token(tokenType);
    pending_nl_count = 0;

    return tokenType;
}

// --- Специфические методы ---

int TokenProcessor::onDecimalLiteral(const char *text) {
    yylval.intLiteral = (int) to_int(text, 10);
    return onToken(DECIMAL_LITERAL);
}

int TokenProcessor::onHexLiteral(const char *text) {
    yylval.intLiteral = (int) to_int(text, 16);
    return onToken(DECIMAL_LITERAL);
}

int TokenProcessor::onBinLiteral(const char *text) {
    yylval.intLiteral = (int) to_int(text, 2);
    return onToken(DECIMAL_LITERAL);
}

int TokenProcessor::onDoubleLiteral(const char *text) {
    yylval.doubleLiteral = to_double(text);
    return onToken(DOUBLE_LITERAL);
}

int TokenProcessor::onBooleanLiteral(int token, bool value) {
    yylval.boolLiteral = value;
    return onToken(token);
}

int TokenProcessor::onStringLiteral(const std::string &text) {
    yylval.stringLiteral = strdup(text.c_str());
    return onToken(yytokentype::STRING_LITERAL);
}

int TokenProcessor::onChar(char c) {
    yylval.charLiteral = c;
    return onToken(yytokentype::CHAR_LITERAL);
}

int TokenProcessor::onID(const char *text) {
    // 1. Сохраняем текст идентификатора в yylval
    yylval.identifier = strdup(text);

    // 2. Определяем тип токена на основе первого символа
    int tokenType = yytokentype::ID; // По умолчанию
    char firstChar = text[0];

    // Если ID состоит из операторных символов, маппим его в конкретный ID_...
    // Важно: это работает только если ID начинается с этих символов.
    // Если ID обычный (буквенный), он останется ID.

    switch (firstChar) {
        case '|': tokenType = ID_VERTICAL_SIGN;
            break;
        case '^': tokenType = ID_CIRCUMFLEX;
            break;
        case '&': tokenType = ID_AMPERSAND;
            break;
        case '=': tokenType = ID_EQUALITY;
            break;
        case '<': tokenType = ID_LESS;
            break;
        case '>': tokenType = ID_GREAT;
            break;
        case ':': tokenType = ID_COLON;
            break;
        case '-': tokenType = ID_MINUS;
            break;
        case '+': tokenType = ID_PLUS;
            break;
        case '*': tokenType = ID_ASTERISK;
            break;
        case '/': tokenType = ID_SLASH;
            break;
        case '%': tokenType = ID_PERCENT;
            break;
        case '!': tokenType = ID_EXCLAMATION;
            break;
        case '~': tokenType = ID_TILDE;
            break;
        // Остальные (например, @, ?, #, \) маппятся в общий ID,
        // если для них нет спец. токенов ID_... в парсере.
        default: tokenType = yytokentype::ID;
            break;
    }

    return onToken(tokenType);
}
