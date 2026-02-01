#include "ConstantPool.h"
#include <cstring>

// Записать uint16_t в Big-Endian
static void writeU16(std::vector<uint8_t>& bytes, uint16_t value) {
    bytes.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
    bytes.push_back(static_cast<uint8_t>(value & 0xFF));
}

// Записать int32_t в Big-Endian
static void writeI32(std::vector<uint8_t>& bytes, int32_t value) {
    bytes.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
    bytes.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
    bytes.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
    bytes.push_back(static_cast<uint8_t>(value & 0xFF));
}

// Записать float в Big-Endian (IEEE 754)
static void writeFloat(std::vector<uint8_t>& bytes, float value) {
    uint32_t bits;
    std::memcpy(&bits, &value, sizeof(bits));
    bytes.push_back(static_cast<uint8_t>((bits >> 24) & 0xFF));
    bytes.push_back(static_cast<uint8_t>((bits >> 16) & 0xFF));
    bytes.push_back(static_cast<uint8_t>((bits >> 8) & 0xFF));
    bytes.push_back(static_cast<uint8_t>(bits & 0xFF));
}

std::vector<uint8_t> ConstantUtf8::toBytes() const {
    std::vector<uint8_t> bytes;
    bytes.push_back(CONSTANT_Utf8);
    writeU16(bytes, static_cast<uint16_t>(value.length()));
    for (char c : value) {
        bytes.push_back(static_cast<uint8_t>(c));
    }
    return bytes;
}

std::string ConstantUtf8::toString() const {
    return "Utf8\t\"" + value + "\"";
}

bool ConstantUtf8::equals(const ConstantInfo& other) const {
    if (other.tag() != CONSTANT_Utf8) return false;
    return value == static_cast<const ConstantUtf8&>(other).value;
}

std::vector<uint8_t> ConstantInt::toBytes() const {
    std::vector<uint8_t> bytes;
    bytes.push_back(CONSTANT_Integer);
    writeI32(bytes, value);
    return bytes;
}

std::string ConstantInt::toString() const {
    return "Integer\t" + std::to_string(value);
}

bool ConstantInt::equals(const ConstantInfo& other) const {
    if (other.tag() != CONSTANT_Integer) return false;
    return value == static_cast<const ConstantInt&>(other).value;
}


std::vector<uint8_t> ConstantFloat::toBytes() const {
    std::vector<uint8_t> bytes;
    bytes.push_back(CONSTANT_Float);
    writeFloat(bytes, value);
    return bytes;
}

std::string ConstantFloat::toString() const {
    return "Float\t" + std::to_string(value);
}

bool ConstantFloat::equals(const ConstantInfo& other) const {
    if (other.tag() != CONSTANT_Float) return false;
    return value == static_cast<const ConstantFloat&>(other).value;
}


std::vector<uint8_t> ConstantString::toBytes() const {
    std::vector<uint8_t> bytes;
    bytes.push_back(CONSTANT_String);
    writeU16(bytes, utf8Ref->index);
    return bytes;
}

std::string ConstantString::toString() const {
    return "String\t#" + std::to_string(utf8Ref->index);
}

bool ConstantString::equals(const ConstantInfo& other) const {
    if (other.tag() != CONSTANT_String) return false;
    return utf8Ref->index == static_cast<const ConstantString&>(other).utf8Ref->index;
}


std::vector<uint8_t> ConstantClass::toBytes() const {
    std::vector<uint8_t> bytes;
    bytes.push_back(CONSTANT_Class);
    writeU16(bytes, nameRef->index);
    return bytes;
}

std::string ConstantClass::toString() const {
    return "Class\t#" + std::to_string(nameRef->index) + "\t// " + nameRef->value;
}

bool ConstantClass::equals(const ConstantInfo& other) const {
    if (other.tag() != CONSTANT_Class) return false;
    return nameRef->index == static_cast<const ConstantClass&>(other).nameRef->index;
}


std::vector<uint8_t> ConstantNameAndType::toBytes() const {
    std::vector<uint8_t> bytes;
    bytes.push_back(CONSTANT_NameAndType);
    writeU16(bytes, nameRef->index);
    writeU16(bytes, descriptorRef->index);
    return bytes;
}

std::string ConstantNameAndType::toString() const {
    return "NameAndType\t#" + std::to_string(nameRef->index) +
           ":#" + std::to_string(descriptorRef->index) +
           "\t// " + nameRef->value + ":" + descriptorRef->value;
}

bool ConstantNameAndType::equals(const ConstantInfo& other) const {
    if (other.tag() != CONSTANT_NameAndType) return false;
    const auto& o = static_cast<const ConstantNameAndType&>(other);
    return nameRef->index == o.nameRef->index &&
           descriptorRef->index == o.descriptorRef->index;
}

std::vector<uint8_t> ConstantFieldRef::toBytes() const {
    std::vector<uint8_t> bytes;
    bytes.push_back(CONSTANT_Fieldref);
    writeU16(bytes, classRef->index);
    writeU16(bytes, nameAndTypeRef->index);
    return bytes;
}

std::string ConstantFieldRef::toString() const {
    return "Fieldref\t#" + std::to_string(classRef->index) +
           ".#" + std::to_string(nameAndTypeRef->index) +
           "\t// " + classRef->nameRef->value + "." +
           nameAndTypeRef->nameRef->value + ":" +
           nameAndTypeRef->descriptorRef->value;
}

bool ConstantFieldRef::equals(const ConstantInfo& other) const {
    if (other.tag() != CONSTANT_Fieldref) return false;
    const auto& o = static_cast<const ConstantFieldRef&>(other);
    return classRef->index == o.classRef->index &&
           nameAndTypeRef->index == o.nameAndTypeRef->index;
}

std::vector<uint8_t> ConstantMethodRef::toBytes() const {
    std::vector<uint8_t> bytes;
    bytes.push_back(CONSTANT_Methodref);
    writeU16(bytes, classRef->index);
    writeU16(bytes, nameAndTypeRef->index);
    return bytes;
}

std::string ConstantMethodRef::toString() const {
    return "Methodref\t#" + std::to_string(classRef->index) +
           ".#" + std::to_string(nameAndTypeRef->index) +
           "\t// " + classRef->nameRef->value + "." +
           nameAndTypeRef->nameRef->value + ":" +
           nameAndTypeRef->descriptorRef->value;
}

bool ConstantMethodRef::equals(const ConstantInfo& other) const {
    if (other.tag() != CONSTANT_Methodref) return false;
    const auto& o = static_cast<const ConstantMethodRef&>(other);
    return classRef->index == o.classRef->index &&
           nameAndTypeRef->index == o.nameAndTypeRef->index;
}
