#include "ConstantPoolBuilder.h"
#include "semantic/tables/tables.hpp"


ConstantPoolBuilder::~ConstantPoolBuilder() {
    for (auto *constant: pool) {
        delete constant;
    }
}

template<typename T>
T *ConstantPoolBuilder::addToPool(T *constant) {
    constant->index = nextIndex++;
    pool.push_back(constant);
    return constant;
}

ConstantUtf8 *ConstantPoolBuilder::addUtf8(const std::string &value) {
    auto it = utf8Cache.find(value);
    if (it != utf8Cache.end()) {
        return it->second;
    }

    auto *constant = new ConstantUtf8(value);
    addToPool(constant);
    utf8Cache[value] = constant;
    return constant;
}

ConstantInt *ConstantPoolBuilder::addInt(int32_t value) {
    auto it = intCache.find(value);
    if (it != intCache.end()) {
        return it->second;
    }

    auto *constant = new ConstantInt(value);
    addToPool(constant);
    intCache[value] = constant;
    return constant;
}

ConstantFloat *ConstantPoolBuilder::addFloat(float value) {
    auto it = floatCache.find(value);
    if (it != floatCache.end()) {
        return it->second;
    }

    auto *constant = new ConstantFloat(value);
    addToPool(constant);
    floatCache[value] = constant;
    return constant;
}


ConstantString *ConstantPoolBuilder::addString(const std::string &value) {
    auto it = stringCache.find(value);
    if (it != stringCache.end()) {
        return it->second;
    }

    ConstantUtf8 *utf8 = addUtf8(value);
    auto *constant = new ConstantString(utf8);
    addToPool(constant);
    stringCache[value] = constant;
    return constant;
}

ConstantClass *ConstantPoolBuilder::addClass(const std::string &className) {
    auto it = classCache.find(className);
    if (it != classCache.end()) {
        return it->second;
    }

    ConstantUtf8 *nameUtf8 = addUtf8(className);
    auto *constant = new ConstantClass(nameUtf8);
    addToPool(constant);
    classCache[className] = constant;
    return constant;
}


ConstantNameAndType *ConstantPoolBuilder::addNameAndType(const std::string &name, const std::string &descriptor) {
    ConstantUtf8 *nameUtf8 = addUtf8(name);
    ConstantUtf8 *descUtf8 = addUtf8(descriptor);

    // Проверка на дубликаты (по индексам)
    for (auto *c: pool) {
        if (c->tag() == CONSTANT_NameAndType) {
            auto *nat = static_cast<ConstantNameAndType *>(c);
            if (nat->nameRef->index == nameUtf8->index &&
                nat->descriptorRef->index == descUtf8->index) {
                return nat;
            }
        }
    }

    auto *constant = new ConstantNameAndType(nameUtf8, descUtf8);
    return addToPool(constant);
}

ConstantFieldRef *ConstantPoolBuilder::addFieldRef(const std::string &className,
                                                   const std::string &fieldName,
                                                   const std::string &descriptor) {
    ConstantClass *classConst = addClass(className);
    ConstantNameAndType *nat = addNameAndType(fieldName, descriptor);

    // Проверка на дубликаты
    for (auto *c: pool) {
        if (c->tag() == CONSTANT_Fieldref) {
            auto *fr = static_cast<ConstantFieldRef *>(c);
            if (fr->classRef->index == classConst->index &&
                fr->nameAndTypeRef->index == nat->index) {
                return fr;
            }
        }
    }

    auto *constant = new ConstantFieldRef(classConst, nat);
    return addToPool(constant);
}

ConstantMethodRef *ConstantPoolBuilder::addMethodRef(const std::string &className,
                                                     const std::string &methodName,
                                                     const std::string &descriptor) {
    ConstantClass *classConst = addClass(className);
    ConstantNameAndType *nat = addNameAndType(methodName, descriptor);

    // Проверка на дубликаты
    for (auto *c: pool) {
        if (c->tag() == CONSTANT_Methodref) {
            auto *mr = static_cast<ConstantMethodRef *>(c);
            if (mr->classRef->index == classConst->index &&
                mr->nameAndTypeRef->index == nat->index) {
                return mr;
            }
        }
    }

    auto *constant = new ConstantMethodRef(classConst, nat);
    return addToPool(constant);
}


ConstantFieldRef *ConstantPoolBuilder::addFieldRef(ClassMetaInfo *cls, FieldMetaInfo *field) {
    std::string className = cls->jvmName;
    std::string fieldName = field->jvmName;
    std::string descriptor = field->dataType.toJvmDescriptor();
    return addFieldRef(className, fieldName, descriptor);
}

ConstantMethodRef *ConstantPoolBuilder::addMethodRef(ClassMetaInfo *cls, MethodMetaInfo *method) {
    std::string className = cls->jvmName;
    std::string methodName = method->jvmName;
    std::string descriptor = method->jvmDescriptor();
    return addMethodRef(className, methodName, descriptor);
}


std::vector<uint8_t> ConstantPoolBuilder::toBytes() const {
    std::vector<uint8_t> bytes;

    // Записываем количество констант + 1 (по спецификации JVM)
    uint16_t count = nextIndex;
    bytes.push_back(static_cast<uint8_t>((count >> 8) & 0xFF));
    bytes.push_back(static_cast<uint8_t>(count & 0xFF));

    // Записываем все константы
    for (const auto *constant: pool) {
        auto constBytes = constant->toBytes();
        bytes.insert(bytes.end(), constBytes.begin(), constBytes.end());
    }

    return bytes;
}

std::string ConstantPoolBuilder::toText() const {
    std::string text = "Constant Pool:\n";
    for (const auto *constant: pool) {
        text += "  #" + std::to_string(constant->index) + " = " + constant->toString() + "\n";
    }
    return text;
}

std::string ConstantPoolBuilder::toCsv() const {
    std::string csv = "Index,Tag,Value\n";
    for (const auto *constant: pool) {
        csv += std::to_string(constant->index) + ",";
        csv += std::to_string(constant->tag()) + ",";
        csv += "\"" + constant->toString() + "\"\n";
    }
    return csv;
}

ConstantInfo *ConstantPoolBuilder::get(uint16_t index) const {
    if (index == 0 || index > pool.size()) {
        return nullptr;
    }
    return pool[index - 1]; // index 1-based, vector 0-based
}
