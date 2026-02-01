#ifndef SCALA_LEXER_CONSTANTPOOL_H
#define SCALA_LEXER_CONSTANTPOOL_H

#include <cstdint>
#include <vector>
#include <string>

// JVM Constant Pool Tags
enum ConstantTag : uint8_t {
    CONSTANT_Utf8 = 1,
    CONSTANT_Integer = 3,
    CONSTANT_Float = 4,
    CONSTANT_Class = 7,
    CONSTANT_String = 8,
    CONSTANT_Fieldref = 9,
    CONSTANT_Methodref = 10,
    CONSTANT_NameAndType = 12
};

// Базовый класс для всех констант
class ConstantInfo {
public:
    uint16_t index = 0;  // Индекс в constant pool (1-based)

    virtual ConstantTag tag() const = 0;
    virtual std::vector<uint8_t> toBytes() const = 0;
    virtual std::string toString() const = 0;
    virtual bool equals(const ConstantInfo& other) const = 0;
    virtual ~ConstantInfo() = default;
};

// ==================== Примитивные константы ====================

class ConstantUtf8 : public ConstantInfo {
public:
    std::string value;

    explicit ConstantUtf8(const std::string& val) : value(val) {}

    ConstantTag tag() const override { return CONSTANT_Utf8; }
    std::vector<uint8_t> toBytes() const override;
    std::string toString() const override;
    bool equals(const ConstantInfo& other) const override;
};

class ConstantInt : public ConstantInfo {
public:
    int32_t value;

    explicit ConstantInt(int32_t val) : value(val) {}

    ConstantTag tag() const override { return CONSTANT_Integer; }
    std::vector<uint8_t> toBytes() const override;
    std::string toString() const override;
    bool equals(const ConstantInfo& other) const override;
};

class ConstantFloat : public ConstantInfo {
public:
    float value;

    explicit ConstantFloat(float val) : value(val) {}

    ConstantTag tag() const override { return CONSTANT_Float; }
    std::vector<uint8_t> toBytes() const override;
    std::string toString() const override;
    bool equals(const ConstantInfo& other) const override;
};

// ==================== Ссылочные константы ====================

class ConstantString : public ConstantInfo {
public:
    ConstantUtf8* utf8Ref;

    explicit ConstantString(ConstantUtf8* ref) : utf8Ref(ref) {}

    ConstantTag tag() const override { return CONSTANT_String; }
    std::vector<uint8_t> toBytes() const override;
    std::string toString() const override;
    bool equals(const ConstantInfo& other) const override;
};

class ConstantClass : public ConstantInfo {
public:
    ConstantUtf8* nameRef;

    explicit ConstantClass(ConstantUtf8* ref) : nameRef(ref) {}

    ConstantTag tag() const override { return CONSTANT_Class; }
    std::vector<uint8_t> toBytes() const override;
    std::string toString() const override;
    bool equals(const ConstantInfo& other) const override;
};

class ConstantNameAndType : public ConstantInfo {
public:
    ConstantUtf8* nameRef;
    ConstantUtf8* descriptorRef;

    ConstantNameAndType(ConstantUtf8* name, ConstantUtf8* descriptor)
        : nameRef(name), descriptorRef(descriptor) {}

    ConstantTag tag() const override { return CONSTANT_NameAndType; }
    std::vector<uint8_t> toBytes() const override;
    std::string toString() const override;
    bool equals(const ConstantInfo& other) const override;
};

class ConstantFieldRef : public ConstantInfo {
public:
    ConstantClass* classRef;
    ConstantNameAndType* nameAndTypeRef;

    ConstantFieldRef(ConstantClass* cls, ConstantNameAndType* nat)
        : classRef(cls), nameAndTypeRef(nat) {}

    ConstantTag tag() const override { return CONSTANT_Fieldref; }
    std::vector<uint8_t> toBytes() const override;
    std::string toString() const override;
    bool equals(const ConstantInfo& other) const override;
};

class ConstantMethodRef : public ConstantInfo {
public:
    ConstantClass* classRef;
    ConstantNameAndType* nameAndTypeRef;

    ConstantMethodRef(ConstantClass* cls, ConstantNameAndType* nat)
        : classRef(cls), nameAndTypeRef(nat) {}

    ConstantTag tag() const override { return CONSTANT_Methodref; }
    std::vector<uint8_t> toBytes() const override;
    std::string toString() const override;
    bool equals(const ConstantInfo& other) const override;
};

#endif // SCALA_LEXER_CONSTANTPOOL_H
