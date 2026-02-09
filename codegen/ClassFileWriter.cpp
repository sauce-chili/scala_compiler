#include "ClassFileWriter.h"
#include "MethodCodeGenerator.h"
#include "semantic/constants/ConstantPoolBuilder.h"
#include "semantic/SemanticContext.h"

#include <fstream>
#include <stdexcept>

// JVM version for Java 8
constexpr uint16_t JAVA_8_MAJOR = 52;
constexpr uint16_t JAVA_8_MINOR = 0;

// Magic number
constexpr uint32_t CLASS_MAGIC = 0xCAFEBABE;

ClassFileWriter::ClassFileWriter(ClassMetaInfo* classInfo)
    : classInfo(classInfo), constantPool(classInfo->constantPool) {
    if (constantPool == nullptr) {
        throw std::runtime_error("ClassMetaInfo must have a ConstantPoolBuilder");
    }
}

std::vector<uint8_t> ClassFileWriter::generate() {
    bytes.clear();

    // Prepare constant pool with known entries
    prepareConstantPool();

    // Pre-generate fields and methods into separate buffers.
    // This may add new entries to the constant pool (e.g., MethodRef for
    // super.<init>()), so it must happen BEFORE we serialize the pool.
    std::vector<uint8_t> fieldsData;
    std::swap(bytes, fieldsData);
    writeFields();
    std::swap(bytes, fieldsData);

    std::vector<uint8_t> methodsData;
    std::swap(bytes, methodsData);
    writeMethods();
    std::swap(bytes, methodsData);

    // Now the constant pool is complete — write the class file in order
    writeMagic();
    writeVersion();
    writeConstantPool();
    writeAccessFlags();
    writeThisClass();
    writeSuperClass();
    writeInterfaces();
    writeBytes(fieldsData);
    writeBytes(methodsData);
    writeClassAttributes();

    return bytes;
}

bool ClassFileWriter::writeToFile(const std::string& path) {
    if (bytes.empty()) {
        generate();
    }

    std::ofstream file(path, std::ios::binary);
    if (!file) {
        return false;
    }

    file.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
    return file.good();
}

// ==================== Helper Methods ====================

void ClassFileWriter::writeU1(uint8_t value) {
    bytes.push_back(value);
}

void ClassFileWriter::writeU2(uint16_t value) {
    bytes.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
    bytes.push_back(static_cast<uint8_t>(value & 0xFF));
}

void ClassFileWriter::writeU4(uint32_t value) {
    bytes.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
    bytes.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
    bytes.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
    bytes.push_back(static_cast<uint8_t>(value & 0xFF));
}

void ClassFileWriter::writeBytes(const std::vector<uint8_t>& data) {
    bytes.insert(bytes.end(), data.begin(), data.end());
}

// ==================== Constant Pool Preparation ====================

void ClassFileWriter::prepareConstantPool() {
    // Add this class
    auto* thisClass = constantPool->addClass(classInfo->jvmName);
    thisClassIndex = thisClass->index;

    // Add super class
    std::string superName = classInfo->parent ? classInfo->parent->jvmName : "java/lang/Object";
    auto* superClass = constantPool->addClass(superName);
    superClassIndex = superClass->index;

    // Add "Code" attribute name
    auto* codeAttr = constantPool->addUtf8("Code");
    codeAttributeNameIndex = codeAttr->index;

    // Add field references
    for (auto& [name, field] : classInfo->fields) {
        constantPool->addFieldRef(classInfo, field);
    }

    // Add method references for all methods
    for (auto& [name, methodList] : classInfo->methods) {
        for (MethodMetaInfo* method : methodList) {
            // Method name and descriptor
            std::string jvmMethodName = (name == "this" || method->isPrimaryConstructor)
                ? "<init>" : method->jvmName;
            constantPool->addUtf8(jvmMethodName);
            constantPool->addUtf8(method->jvmDescriptor());
        }
    }
}

// ==================== Structure Writers ====================

void ClassFileWriter::writeMagic() {
    writeU4(CLASS_MAGIC);
}

void ClassFileWriter::writeVersion() {
    writeU2(JAVA_8_MINOR);
    writeU2(JAVA_8_MAJOR);
}

void ClassFileWriter::writeConstantPool() {
    // constant_pool_count = number of entries + 1
    writeU2(constantPool->size() + 1);

    // Write constant pool entries
    writeBytes(constantPool->toBytes());
}

void ClassFileWriter::writeAccessFlags() {
    uint16_t flags = AccessFlags::ACC_SUPER;  // Always set for classes

    // Map Scala modifiers to JVM flags
    if (!classInfo->isPrivate() && !classInfo->isProtected()) {
        flags |= AccessFlags::ACC_PUBLIC;
    }
    if (classInfo->isFinal()) {
        flags |= AccessFlags::ACC_FINAL;
    }
    if (classInfo->modifiers.hasModifier(_ABSTRACT)) {
        flags |= AccessFlags::ACC_ABSTRACT;
    }

    writeU2(flags);
}

void ClassFileWriter::writeThisClass() {
    writeU2(thisClassIndex);
}

void ClassFileWriter::writeSuperClass() {
    writeU2(superClassIndex);
}

void ClassFileWriter::writeInterfaces() {
    // interfaces_count = 0 (no trait support yet)
    writeU2(0);
}

void ClassFileWriter::writeFields() {
    uint16_t fieldCount = static_cast<uint16_t>(classInfo->fields.size());
    if (singletonMode) {
        fieldCount++;  // MODULE$ static field
    }
    writeU2(fieldCount);

    for (auto& [name, field] : classInfo->fields) {
        writeField(field);
    }

    if (singletonMode) {
        // Write MODULE$ field: public static final <jvmName>
        writeU2(AccessFlags::ACC_PUBLIC | AccessFlags::ACC_STATIC | AccessFlags::ACC_FINAL);
        auto* nameUtf8 = constantPool->addUtf8("MODULE$");
        writeU2(nameUtf8->index);
        std::string descriptor = "L" + classInfo->jvmName + ";";
        auto* descUtf8 = constantPool->addUtf8(descriptor);
        writeU2(descUtf8->index);
        writeU2(0);  // no attributes
    }
}

void ClassFileWriter::writeField(FieldMetaInfo* field) {
    // access_flags
    writeU2(getFieldAccessFlags(field));

    // name_index
    auto* nameUtf8 = constantPool->addUtf8(field->jvmName);
    writeU2(nameUtf8->index);

    // descriptor_index
    auto* descUtf8 = constantPool->addUtf8(field->dataType.toJvmDescriptor());
    writeU2(descUtf8->index);

    // attributes_count = 0 (no ConstantValue for now)
    writeU2(0);
}

uint16_t ClassFileWriter::getFieldAccessFlags(FieldMetaInfo* field) {
    uint16_t flags = 0;

    if (field->isPrivate()) {
        flags |= AccessFlags::ACC_PRIVATE;
    } else if (field->isProtected()) {
        flags |= AccessFlags::ACC_PROTECTED;
    } else {
        flags |= AccessFlags::ACC_PUBLIC;
    }

    if (field->isVal) {
        flags |= AccessFlags::ACC_FINAL;
    }

    return flags;
}

void ClassFileWriter::writeMethods() {
    // Count total methods including constructors
    uint16_t methodCount = 0;
    for (auto& [name, methodList] : classInfo->methods) {
        methodCount += methodList.size();
    }

    // Always need at least one constructor
    bool hasConstructor = classInfo->methods.count("this") > 0;
    if (!hasConstructor) {
        methodCount++;  // Will generate default constructor
    }

    if (singletonMode) {
        methodCount++;  // <clinit> for MODULE$ initialization
    }

    writeU2(methodCount);

    // Write constructors
    if (hasConstructor) {
        for (MethodMetaInfo* ctor : classInfo->methods["this"]) {
            writeMethod(ctor);
        }
    } else {
        // Generate default constructor
        writeDefaultConstructor();
    }

    // Write other methods
    for (auto& [name, methodList] : classInfo->methods) {
        if (name == "this") continue;  // Already handled

        for (MethodMetaInfo* method : methodList) {
            writeMethod(method);
        }
    }

    if (singletonMode) {
        // Generate <clinit>: static initializer that creates the singleton
        // public static void <clinit>() {
        //   new <this>; dup; invokespecial <init>()V; putstatic MODULE$; return
        // }
        writeU2(AccessFlags::ACC_STATIC);

        auto* clinitName = constantPool->addUtf8("<clinit>");
        writeU2(clinitName->index);

        auto* clinitDesc = constantPool->addUtf8("()V");
        writeU2(clinitDesc->index);

        // attributes_count = 1 (Code)
        writeU2(1);

        // Build clinit bytecode manually
        std::vector<uint8_t> codeBytes;
        // new <this>
        codeBytes.push_back(static_cast<uint8_t>(Instruction::new_));
        codeBytes.push_back(static_cast<uint8_t>((thisClassIndex >> 8) & 0xFF));
        codeBytes.push_back(static_cast<uint8_t>(thisClassIndex & 0xFF));
        // dup
        codeBytes.push_back(static_cast<uint8_t>(Instruction::dup));
        // invokespecial <init>()V
        auto* initRef = constantPool->addMethodRef(classInfo->jvmName, "<init>", "()V");
        codeBytes.push_back(static_cast<uint8_t>(Instruction::invokespecial));
        codeBytes.push_back(static_cast<uint8_t>((initRef->index >> 8) & 0xFF));
        codeBytes.push_back(static_cast<uint8_t>(initRef->index & 0xFF));
        // putstatic MODULE$
        std::string moduleDesc = "L" + classInfo->jvmName + ";";
        auto* moduleFieldRef = constantPool->addFieldRef(classInfo->jvmName, "MODULE$", moduleDesc);
        codeBytes.push_back(static_cast<uint8_t>(Instruction::putstatic));
        codeBytes.push_back(static_cast<uint8_t>((moduleFieldRef->index >> 8) & 0xFF));
        codeBytes.push_back(static_cast<uint8_t>(moduleFieldRef->index & 0xFF));
        // return
        codeBytes.push_back(static_cast<uint8_t>(Instruction::return_));

        // Build Code attribute
        std::vector<uint8_t> codeAttr;
        // max_stack = 2 (new + dup)
        codeAttr.push_back(0); codeAttr.push_back(2);
        // max_locals = 0 (static method, no locals)
        codeAttr.push_back(0); codeAttr.push_back(0);
        // code_length
        uint32_t codeLen = codeBytes.size();
        codeAttr.push_back(static_cast<uint8_t>((codeLen >> 24) & 0xFF));
        codeAttr.push_back(static_cast<uint8_t>((codeLen >> 16) & 0xFF));
        codeAttr.push_back(static_cast<uint8_t>((codeLen >> 8) & 0xFF));
        codeAttr.push_back(static_cast<uint8_t>(codeLen & 0xFF));
        // code[]
        codeAttr.insert(codeAttr.end(), codeBytes.begin(), codeBytes.end());
        // exception_table_length = 0
        codeAttr.push_back(0); codeAttr.push_back(0);
        // attributes_count = 0
        codeAttr.push_back(0); codeAttr.push_back(0);

        // attribute_name_index ("Code")
        writeU2(codeAttributeNameIndex);
        // attribute_length
        writeU4(static_cast<uint32_t>(codeAttr.size()));
        // Code attribute data
        writeBytes(codeAttr);
    }
}

void ClassFileWriter::writeMethod(MethodMetaInfo* method) {
    // access_flags
    writeU2(getMethodAccessFlags(method));

    // name_index
    std::string methodName = (method->name == "this" || method->isPrimaryConstructor)
        ? "<init>" : method->jvmName;
    auto* nameUtf8 = constantPool->addUtf8(methodName);
    writeU2(nameUtf8->index);

    // descriptor_index
    auto* descUtf8 = constantPool->addUtf8(method->jvmDescriptor());
    writeU2(descUtf8->index);

    // attributes_count
    bool hasBody = (method->body != nullptr) || method->isPrimaryConstructor ||
                   (method->name == "this");

    if (hasBody) {
        writeU2(1);  // One attribute: Code

        // Generate Code attribute
        std::vector<uint8_t> codeAttr = generateCodeAttribute(method);

        // attribute_name_index ("Code")
        writeU2(codeAttributeNameIndex);

        // attribute_length
        writeU4(static_cast<uint32_t>(codeAttr.size()));

        // Code attribute data
        writeBytes(codeAttr);
    } else {
        // Abstract method - no Code attribute
        writeU2(0);
    }
}

void ClassFileWriter::writeDefaultConstructor() {
    // access_flags = public
    writeU2(AccessFlags::ACC_PUBLIC);

    // name_index = "<init>"
    auto* nameUtf8 = constantPool->addUtf8("<init>");
    writeU2(nameUtf8->index);

    // descriptor_index = "()V"
    auto* descUtf8 = constantPool->addUtf8("()V");
    writeU2(descUtf8->index);

    // attributes_count = 1 (Code)
    writeU2(1);

    // Generate default constructor code
    MethodCodeGenerator gen(nullptr, classInfo, constantPool);
    gen.generateDefaultConstructor();

    std::vector<uint8_t> codeAttr = gen.generateCodeAttribute();

    // attribute_name_index
    writeU2(codeAttributeNameIndex);

    // attribute_length
    writeU4(static_cast<uint32_t>(codeAttr.size()));

    // Code data
    writeBytes(codeAttr);
}

uint16_t ClassFileWriter::getMethodAccessFlags(MethodMetaInfo* method) {
    uint16_t flags = 0;

    if (method->isPrivate()) {
        flags |= AccessFlags::ACC_PRIVATE;
    } else if (method->isProtected()) {
        flags |= AccessFlags::ACC_PROTECTED;
    } else {
        flags |= AccessFlags::ACC_PUBLIC;
    }

    if (method->isFinal()) {
        flags |= AccessFlags::ACC_FINAL;
    }

    if (method->body == nullptr && !method->isPrimaryConstructor && method->name != "this") {
        flags |= AccessFlags::ACC_ABSTRACT;
    }

    return flags;
}

std::vector<uint8_t> ClassFileWriter::generateCodeAttribute(MethodMetaInfo* method) {
    MethodCodeGenerator gen(method, classInfo, constantPool);

    if (method->isPrimaryConstructor || method->name == "this") {
        gen.generatePrimaryConstructor();
    } else {
        gen.generate();
    }

    return gen.generateCodeAttribute();
}

void ClassFileWriter::writeClassAttributes() {
    // For now, no class attributes
    // Could add SourceFile attribute later
    writeU2(0);
}
