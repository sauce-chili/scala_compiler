//
// Created by Dmitry Smirnov on 05.01.2026.
//

#include "TokenProcessor.h"
#include <algorithm>
#include <cstring>
#include <iostream>

extern "C" int real_lineno;

extern const char *get_bison_token_name(int token);

TokenProcessor::TokenProcessor(BufferedYYLex &lexer) : bufferedLexer(lexer) {
    nlEnabledStack.push_back(true); // Изначально переводы строк разрешены
}

std::string TokenProcessor::removeUnderscore(const char *text) {
    std::string s(text);
    s.erase(std::ranges::remove(s, '_').begin(), s.end());
    return s;
}

long long TokenProcessor::toInt(const char *text, int base) {
    std::string s = removeUnderscore(text);
    if (base == 2) {
        if (s.rfind("0b", 0) == 0 || s.rfind("0B", 0) == 0) s = s.substr(2);
        long long val = 0;
        for (char c: s) val = (val << 1) | (c - '0');
        return val;
    }
    return strtoll(s.c_str(), nullptr, base);
}

double TokenProcessor::toDouble(const char *text) {
    std::string s = removeUnderscore(text);
    return atof(s.c_str());
}

bool TokenProcessor::canBeginStmt(const int token) {
    switch (token) {
        // каких-то токенов нет(на пример match или forSome, но мы их и не поддерживаем)
        // case CATCH:
        case ELSE:
        case EXTENDS:
        // case FINALLY:
        // case WITH:
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


bool TokenProcessor::canEndStmt(const int token) {
    switch (token) {
        // каких-то токенов нет(на пример type, но мы их и не поддерживаем)
        case THIS:
        case NULL_LITERAL:
        case TRUE_LITERAL:
        case FALSE_LITERAL:
        case RETURN:
        case '_':
        case ')':
        case ']':
        case '}':
        // вариации id:
        case ID:
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
        case DOUBLE_LITERAL:
        case STRING_LITERAL:
        case CHAR_LITERAL:
            return true;
        default: return false;
    }
}

void TokenProcessor::updateStateByCurrentToken(const TokenInfo &info) {
    // Обновляем стек скобок (nl zone enable)
    if (info.type == '(' || info.type == '[') nlEnabledStack.push_back(false);
    else if (info.type == '{') nlEnabledStack.push_back(true);
    else if ((info.type == ')' || info.type == ']') && !nlEnabledStack.empty()) nlEnabledStack.pop_back();
    else if (info.type == '}' && !nlEnabledStack.empty()) nlEnabledStack.pop_back();

    lastToken = info;
    pendingNlCount = 0;
}

void TokenProcessor::onNewLine() {
    if (pendingNlCount < 2) {
        pendingNlCount++;
    }
    // Ничего не возвращаем, лексер Flex просто продолжит работу
}

void TokenProcessor::onSemicolon() {
    pendingSemicolonCount++;
    pendingNlCount = 0; // тк не может начинать выражение
}

int TokenProcessor::onToken(int tokenType) {
    TokenInfo currentToken{
        tokenType, yylval, real_lineno, get_bison_token_name(tokenType)
    };

    YYSTYPE emptyVal{};
    // Обнуляем все поля union, чтобы там не было случайных указателей
    std::memset(&emptyVal, 0, sizeof(YYSTYPE));

    // вставляем все отложенные ";"
    if (pendingSemicolonCount > 0) {
        // Если текущий токен ELSE, то "съедаем" одну точку с запятой
        if (currentToken.type == ELSE) {
            pendingSemicolonCount--;
        }

        if (pendingSemicolonCount > 0) {

            TokenInfo semicolonToken = {';', emptyVal, real_lineno, ";"};

            pendingSemicolonCount--; // тк один ";" мы будем возвращать
            for (; pendingSemicolonCount > 0; pendingSemicolonCount--) {
                bufferedLexer.push(semicolonToken);
            }
            // закидываем текущий токен для возврата его парсеру после вставок всех ";"
            bufferedLexer.push(currentToken);
            // вставок nl не нужно, тк вместо него были вставлены ";", также после ";" не может быть вставлен nl,
            // поэтому обновляем состояния по текущему токену
            updateStateByCurrentToken(currentToken);
            // pendingNlCount = 0; // не имеют значения, тк ставим прямой символ окончания statement

            // !!! ВАЖНО: Обнуляем глобальный yylval перед возвратом NL
            yylval = emptyVal;

            return semicolonToken.type;
        } else {
            // здесь можем оказаться только если зашли в ветку else выше;
            // else не может начинать выражение,
            // значит вставок nl не требуется и сразу возвращаем токен
            updateStateByCurrentToken(currentToken);
            return currentToken.type;
        }
    }

    // 1. Проверяем условия вставки NL
    // https://scala-lang.org/files/archive/spec/2.13/01-lexical-syntax.html#newline-characters
    bool nlZoneEnabled = nlEnabledStack.empty() || nlEnabledStack.back();
    bool shouldInsertNL =
            pendingNlCount > 0 &&
            nlZoneEnabled &&
            canEndStmt(lastToken.type) && // YYEMPTY сюда тоже попадает (вернет false)
            canBeginStmt(currentToken.type);

    if (shouldInsertNL) {
        if (pendingNlCount > 1) {
            // Для 2 и более NL: кладём ещё один NL в очередь
            bufferedLexer.push({NL, emptyVal, real_lineno, "NL"});
        }

        // Текущий токен всегда уходит в очередь
        bufferedLexer.push(currentToken);
        updateStateByCurrentToken(currentToken);

        // pendingNlCount = 0;

        // !!! ВАЖНО: Обнуляем глобальный yylval перед возвратом NL
        yylval = emptyVal;

        // Возвращаем вставленный NL
        return NL;
    }

    // 2. NL не нужен — обычная обработка токена
    updateStateByCurrentToken(currentToken);
    // pendingNlCount = 0;

    return currentToken.type;
}

// --- Специфические методы ---

int TokenProcessor::onDecimalLiteral(const char *text) {
    yylval.intLiteral = (int) toInt(text, 10);
    return onToken(DECIMAL_LITERAL);
}

int TokenProcessor::onHexLiteral(const char *text) {
    yylval.intLiteral = (int) toInt(text, 16);
    return onToken(DECIMAL_LITERAL);
}

int TokenProcessor::onBinLiteral(const char *text) {
    yylval.intLiteral = (int) toInt(text, 2);
    return onToken(DECIMAL_LITERAL);
}

int TokenProcessor::onDoubleLiteral(const char *text) {
    yylval.doubleLiteral = toDouble(text);
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
    return onToken(CHAR_LITERAL);
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
        default: tokenType = ID;
            break;
    }

    return onToken(tokenType);
}
