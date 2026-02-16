#ifndef SCALA_LEXER_MAINFORWARDERWRITER_H
#define SCALA_LEXER_MAINFORWARDERWRITER_H

#include <cstdint>
#include <string>
#include <vector>

/**
 * MainForwarderWriter - generates a thin forwarder A.class
 *
 * The forwarder class has:
 * - Default constructor <init>()V
 * - public static void main(String[] args) that delegates to A$.MODULE$.main(args)
 *
 * This follows the Scala object pattern where the real implementation
 * lives in A$.class as instance methods on a singleton.
 */
class MainForwarderWriter {
private:
    std::string className;      // e.g. "A"
    std::string implClassName;  // e.g. "A$"
    std::vector<uint8_t> bytes;

    void writeU1(uint8_t value);
    void writeU2(uint16_t value);
    void writeU4(uint32_t value);
    void writeBytes(const std::vector<uint8_t>& data);

public:
    MainForwarderWriter(const std::string& className, const std::string& implClassName);

    std::vector<uint8_t> generate();
    bool writeToFile(const std::string& path);
};

#endif // SCALA_LEXER_MAINFORWARDERWRITER_H
