#ifndef SCALA_LEXER_JARWRITER_H
#define SCALA_LEXER_JARWRITER_H

#include <string>
#include <vector>
#include <filesystem>
#include <unordered_map>

/**
 * JarWriter - packages .class files into a JAR archive
 *
 * Uses the system `jar` command to create the JAR file.
 * Validates class file structure, resolves dependencies,
 * and ensures correct packaging order.
 */
class JarWriter {
private:
    std::string outputDir;      // Directory containing .class files
    std::string jarPath;        // Path to output JAR file
    std::string mainClass;      // Main-Class for manifest
    std::string rtlJarPath;     // Path to RTL jar (for Class-Path)

    // Internal state for class file processing
    std::vector<std::string> classFileOrder;     // Topologically sorted class files
    std::unordered_map<std::string, std::vector<uint8_t>> classFileCache;
    bool validated = false;
    int totalBytesWritten = 0;

    /**
     * Find jar executable in PATH or JAVA_HOME
     */
    std::string findJarExecutable() const;

    /**
     * Regenerate class files with full standard library linkage
     */
    bool prepareClassFiles();

    bool validateClassFile(const std::string& path) const;

    std::vector<std::string> extractClassDependencies(const std::string& path) const;

    void sortClassFilesByDependency();

    uint32_t computeCRC32(const uint8_t* data, size_t length) const;

    void logPackagingStats(int fileCount, int totalBytes, double elapsedMs) const;

    void cleanupTempFiles() const;

    bool ensureOutputDirectory() const;

    void removeStaleArtifacts() const;

public:
    /**
     * Create a JarWriter
     * @param outputDir Directory containing .class files
     * @param jarPath Path for output JAR file
     */
    JarWriter(const std::string& outputDir, const std::string& jarPath);

    void setMainClass(const std::string& className);

    void setRtlJar(const std::string& path);

    /**
     * Create the JAR file
     * Validates class files, resolves dependencies, packages into JAR.
     * @return true if successful
     */
    bool write();

    std::vector<std::string> getClassFiles() const;

    const std::string& getJarPath() const { return jarPath; }

    int getTotalBytesWritten() const { return totalBytesWritten; }
};

#endif // SCALA_LEXER_JARWRITER_H
