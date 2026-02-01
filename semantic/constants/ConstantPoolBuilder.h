#ifndef SCALA_LEXER_CONSTANTPOOLBUILDER_H
#define SCALA_LEXER_CONSTANTPOOLBUILDER_H

#include "ConstantPool.h"
#include <map>
#include <string>
#include <vector>

class ClassMetaInfo;
class FieldMetaInfo;
class MethodMetaInfo;

class ConstantPoolBuilder {
private:
    std::vector<ConstantInfo*> pool;  // Индексация с 0, но index константы начинается с 1
    uint16_t nextIndex = 1;

    // Кэш для дедупликации
    std::map<std::string, ConstantUtf8*> utf8Cache;
    std::map<std::string, ConstantClass*> classCache;
    std::map<int32_t, ConstantInt*> intCache;
    std::map<float, ConstantFloat*> floatCache;
    std::map<std::string, ConstantString*> stringCache;

    // Вспомогательный метод для добавления константы в пул
    template<typename T>
    T* addToPool(T* constant);

public:
    ConstantPoolBuilder() = default;
    ~ConstantPoolBuilder();

    // Добавление примитивных констант (с дедупликацией)
    ConstantUtf8* addUtf8(const std::string& value);
    ConstantInt* addInt(int32_t value);
    ConstantFloat* addFloat(float value);

    // Добавление ссылочных констант
    ConstantString* addString(const std::string& value);
    ConstantClass* addClass(const std::string& className);

    // Добавление структурных констант
    ConstantNameAndType* addNameAndType(const std::string& name, const std::string& descriptor);
    ConstantFieldRef* addFieldRef(const std::string& className, const std::string& fieldName, const std::string& descriptor);
    ConstantMethodRef* addMethodRef(const std::string& className, const std::string& methodName, const std::string& descriptor);

    // Добавление ссылок из метаданных
    ConstantFieldRef* addFieldRef(ClassMetaInfo* cls, FieldMetaInfo* field);
    ConstantMethodRef* addMethodRef(ClassMetaInfo* cls, MethodMetaInfo* method);

    // Сериализация всего пула в байты
    std::vector<uint8_t> toBytes() const;

    // Экспорт в текстовый формат (для отладки)
    std::string toText() const;

    // Экспорт в CSV
    std::string toCsv() const;

    // Получение константы по индексу (1-based)
    ConstantInfo* get(uint16_t index) const;

    // Размер пула
    uint16_t size() const { return nextIndex - 1; }

    // Получить все константы
    const std::vector<ConstantInfo*>& getPool() const { return pool; }
};

#endif // SCALA_LEXER_CONSTANTPOOLBUILDER_H
