//
// Created by Dmitry Smirnov on 18.01.2026.
//

#ifndef SCALA_LEXER_SEMANTICCONTEXT_H
#define SCALA_LEXER_SEMANTICCONTEXT_H
#include <optional>
#include <string>
#include <unordered_map>

class RtlClassMetaInfo;
class ClassDefNode;
using namespace std;

class ClassMetaInfo;

class SemanticContext {
    SemanticContext() = default;
    bool rtlInitialized = false;

public:
    unordered_map<string, ClassMetaInfo *> classes;

    /**
     * Возвращает созданный класс, если такого нет
     */
    optional<ClassMetaInfo *> addClass(ClassDefNode *classDefNode);

    /**
     * Возвращает созданный класс, если такого нет
     */
    optional<ClassMetaInfo *> addClass(ClassMetaInfo *classMetaInfo);

    /**
     * Добавляет RTL класс в контекст
     */
    optional<ClassMetaInfo*> addRTL(RtlClassMetaInfo* rtlClass);

    /**
     * Инициализирует RTL классы (вызывается один раз)
     */
    void ensureRtlInitialized();

    static SemanticContext &instance() {
        static SemanticContext instance;
        return instance;
    }

    SemanticContext(const SemanticContext &) = delete;

    SemanticContext &operator=(const SemanticContext &) = delete;

    SemanticContext(SemanticContext &&) = delete;

    SemanticContext &operator=(SemanticContext &&) = delete;
};

// для быстрого доступа из любой точки
#define ctx() SemanticContext::instance()


#endif //SCALA_LEXER_SEMANTICCONTEXT_H
