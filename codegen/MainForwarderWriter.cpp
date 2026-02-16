#include "MainForwarderWriter.h"
#include "AccessFlags.h"
#include "Instructions.h"

#include <fstream>

// JVM constants
constexpr uint32_t FORWARDER_MAGIC = 0xCAFEBABE;
constexpr uint16_t FORWARDER_MINOR = 0;
constexpr uint16_t FORWARDER_MAJOR = 52;  // Java 8

MainForwarderWriter::MainForwarderWriter(const std::string& className, const std::string& implClassName)
    : className(className), implClassName(implClassName) {
}

void MainForwarderWriter::writeU1(uint8_t value) {
    bytes.push_back(value);
}

void MainForwarderWriter::writeU2(uint16_t value) {
    bytes.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
    bytes.push_back(static_cast<uint8_t>(value & 0xFF));
}

void MainForwarderWriter::writeU4(uint32_t value) {
    bytes.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
    bytes.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
    bytes.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
    bytes.push_back(static_cast<uint8_t>(value & 0xFF));
}

void MainForwarderWriter::writeBytes(const std::vector<uint8_t>& data) {
    bytes.insert(bytes.end(), data.begin(), data.end());
}

std::vector<uint8_t> MainForwarderWriter::generate() {
    bytes.clear();

    // Build constant pool entries manually (1-based indexing)
    // We need:
    //  1: Utf8 "<init>"
    //  2: Utf8 "()V"
    //  3: Utf8 "Code"
    //  4: Utf8 className (e.g. "A")
    //  5: Class #4 (this class)
    //  6: Utf8 "java/lang/Object"
    //  7: Class #6 (super class)
    //  8: NameAndType #1:#2 (<init>:()V)
    //  9: MethodRef #7.#8 (Object.<init>()V)
    // 10: Utf8 "main"
    // 11: Utf8 "([Ljava/lang/String;)V"
    // 12: Utf8 implClassName (e.g. "A$")
    // 13: Class #12 (impl class)
    // 14: Utf8 "MODULE$"
    // 15: Utf8 "L<implClassName>;"
    // 16: NameAndType #14:#15 (MODULE$:LA$;)
    // 17: FieldRef #13.#16 (A$.MODULE$:LA$;)
    // 18: NameAndType #10:#11 (main:([Ljava/lang/String;)V)
    // 19: MethodRef #13.#18 (A$.main:([Ljava/lang/String;)V)

    std::string implDescriptor = "L" + implClassName + ";";

    // --- Magic & Version ---
    writeU4(FORWARDER_MAGIC);
    writeU2(FORWARDER_MINOR);
    writeU2(FORWARDER_MAJOR);

    // --- Constant Pool ---
    // constant_pool_count = 19 + 1 = 20
    writeU2(20);

    // #1: Utf8 "<init>"
    writeU1(1); // CONSTANT_Utf8
    std::string s1 = "<init>";
    writeU2(static_cast<uint16_t>(s1.size()));
    for (char c : s1) writeU1(static_cast<uint8_t>(c));

    // #2: Utf8 "()V"
    writeU1(1);
    std::string s2 = "()V";
    writeU2(static_cast<uint16_t>(s2.size()));
    for (char c : s2) writeU1(static_cast<uint8_t>(c));

    // #3: Utf8 "Code"
    writeU1(1);
    std::string s3 = "Code";
    writeU2(static_cast<uint16_t>(s3.size()));
    for (char c : s3) writeU1(static_cast<uint8_t>(c));

    // #4: Utf8 className
    writeU1(1);
    writeU2(static_cast<uint16_t>(className.size()));
    for (char c : className) writeU1(static_cast<uint8_t>(c));

    // #5: Class #4
    writeU1(7); // CONSTANT_Class
    writeU2(4);

    // #6: Utf8 "java/lang/Object"
    writeU1(1);
    std::string s6 = "java/lang/Object";
    writeU2(static_cast<uint16_t>(s6.size()));
    for (char c : s6) writeU1(static_cast<uint8_t>(c));

    // #7: Class #6
    writeU1(7);
    writeU2(6);

    // #8: NameAndType #1:#2  (<init>:()V)
    writeU1(12); // CONSTANT_NameAndType
    writeU2(1);
    writeU2(2);

    // #9: MethodRef #7.#8  (Object.<init>()V)
    writeU1(10); // CONSTANT_MethodRef
    writeU2(7);
    writeU2(8);

    // #10: Utf8 "main"
    writeU1(1);
    std::string s10 = "main";
    writeU2(static_cast<uint16_t>(s10.size()));
    for (char c : s10) writeU1(static_cast<uint8_t>(c));

    // #11: Utf8 "([Ljava/lang/String;)V"
    writeU1(1);
    std::string s11 = "([Ljava/lang/String;)V";
    writeU2(static_cast<uint16_t>(s11.size()));
    for (char c : s11) writeU1(static_cast<uint8_t>(c));

    // #12: Utf8 implClassName
    writeU1(1);
    writeU2(static_cast<uint16_t>(implClassName.size()));
    for (char c : implClassName) writeU1(static_cast<uint8_t>(c));

    // #13: Class #12
    writeU1(7);
    writeU2(12);

    // #14: Utf8 "MODULE$"
    writeU1(1);
    std::string s14 = "MODULE$";
    writeU2(static_cast<uint16_t>(s14.size()));
    for (char c : s14) writeU1(static_cast<uint8_t>(c));

    // #15: Utf8 implDescriptor (e.g. "LA$;")
    writeU1(1);
    writeU2(static_cast<uint16_t>(implDescriptor.size()));
    for (char c : implDescriptor) writeU1(static_cast<uint8_t>(c));

    // #16: NameAndType #14:#15  (MODULE$:LA$;)
    writeU1(12);
    writeU2(14);
    writeU2(15);

    // #17: FieldRef #13.#16  (A$.MODULE$)
    writeU1(9); // CONSTANT_FieldRef
    writeU2(13);
    writeU2(16);

    // #18: NameAndType #10:#11  (main:([Ljava/lang/String;)V)
    writeU1(12);
    writeU2(10);
    writeU2(11);

    // #19: MethodRef #13.#18  (A$.main())
    writeU1(10);
    writeU2(13);
    writeU2(18);

    // --- Access Flags ---
    writeU2(AccessFlags::ACC_PUBLIC | AccessFlags::ACC_SUPER | AccessFlags::ACC_FINAL);

    // --- This Class ---
    writeU2(5);  // #5

    // --- Super Class ---
    writeU2(7);  // #7

    // --- Interfaces ---
    writeU2(0);

    // --- Fields ---
    writeU2(0);

    // --- Methods (2: <init> and main) ---
    writeU2(2);

    // Method 1: <init>()V
    {
        writeU2(AccessFlags::ACC_PUBLIC);  // access_flags
        writeU2(1);   // name_index: #1 "<init>"
        writeU2(2);   // descriptor_index: #2 "()V"
        writeU2(1);   // attributes_count: 1 (Code)

        // Code attribute
        std::vector<uint8_t> code;
        // aload_0
        code.push_back(static_cast<uint8_t>(Instruction::aload_0));
        // invokespecial #9 (Object.<init>()V)
        code.push_back(static_cast<uint8_t>(Instruction::invokespecial));
        code.push_back(0x00); code.push_back(0x09);
        // return
        code.push_back(static_cast<uint8_t>(Instruction::return_));

        // Code attribute structure
        writeU2(3);  // attribute_name_index: #3 "Code"
        uint32_t attrLen = 2 + 2 + 4 + code.size() + 2 + 2;  // max_stack + max_locals + code_length + code + exception_table_length + attributes_count
        writeU4(attrLen);
        writeU2(1);  // max_stack
        writeU2(1);  // max_locals
        writeU4(static_cast<uint32_t>(code.size()));
        writeBytes(code);
        writeU2(0);  // exception_table_length
        writeU2(0);  // attributes_count
    }

    // Method 2: public static void main(String[])
    {
        writeU2(AccessFlags::ACC_PUBLIC | AccessFlags::ACC_STATIC);  // access_flags
        writeU2(10);  // name_index: #10 "main"
        writeU2(11);  // descriptor_index: #11 "([Ljava/lang/String;)V"
        writeU2(1);   // attributes_count: 1 (Code)

        // Code:
        //   getstatic  #17  (A$.MODULE$:LA$;)
        //   aload_0          (args is in slot 0 for static method)
        //   invokevirtual #19 (A$.main:([Ljava/lang/String;)V)
        //   return
        std::vector<uint8_t> code;
        // getstatic #17
        code.push_back(static_cast<uint8_t>(Instruction::getstatic));
        code.push_back(0x00); code.push_back(0x11);  // #17
        // aload_0 (args)
        code.push_back(static_cast<uint8_t>(Instruction::aload_0));
        // invokevirtual #19
        code.push_back(static_cast<uint8_t>(Instruction::invokevirtual));
        code.push_back(0x00); code.push_back(0x13);  // #19
        // return
        code.push_back(static_cast<uint8_t>(Instruction::return_));

        // Code attribute structure
        writeU2(3);  // attribute_name_index: #3 "Code"
        uint32_t attrLen = 2 + 2 + 4 + code.size() + 2 + 2;
        writeU4(attrLen);
        writeU2(2);  // max_stack (getstatic pushes 1, aload pushes 1)
        writeU2(1);  // max_locals (args in slot 0)
        writeU4(static_cast<uint32_t>(code.size()));
        writeBytes(code);
        writeU2(0);  // exception_table_length
        writeU2(0);  // attributes_count
    }

    // --- Class Attributes ---
    writeU2(0);

    return bytes;
}

bool MainForwarderWriter::writeToFile(const std::string& path) {
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
