//
// Created by Dmitry Smirnov on 13.01.2026.
//

#ifndef SCALA_LEXER_IEXPR_H
#define SCALA_LEXER_IEXPR_H

#include "semantic/tools/datatype.h"

class ClassMetaInfo;
class MethodMetaInfo;
class Scope;

constexpr int PARENTS_CONSIDER = 99999;

/**
 * @brief Интерфейс для всех узлов-выражений в AST.
 *
 * Предоставляет интерфейс для вывода типа. Каждый узел выражения
 * должен реализовывать этот интерфейс и уметь вычислять свой тип.
 */
class IExpr {
public:
    /**
     * @brief Определяет тип данного выражения.
     *
     * Анализирует узел AST и возвращает тип данных, который это выражение
     * будет иметь во время выполнения. Для корректного вывода типа
     * требуется доступ к контексту семантического анализа.
     *
     * @param currentClass Текущий класс, в рамках которого выполняется анализ
     * @param currentMethod Текущий метод, если анализ выполняется внутри метода (может быть nullptr)
     * @param currentScope Текущая область видимости для разрешения локальных переменных (может быть nullptr)
     * @return DataType Тип выражения
     *
     * @throws SemanticError если тип не может быть определён или обнаружена ошибка типов
     *
     * @see DataType
     * @see ClassMetaInfo
     * @see MethodMetaInfo
     * @see Scope
     */
    virtual DataType inferType(
        ClassMetaInfo *currentClass,
        MethodMetaInfo *currentMethod,
        Scope *currentScope,
        int parentsConsider = PARENTS_CONSIDER
    ) const = 0;

    virtual ~IExpr() = default;
};

#endif //SCALA_LEXER_IEXPR_H
