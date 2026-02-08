#ifndef SCALA_LEXER_JARWRITER_H
#define SCALA_LEXER_JARWRITER_H

#include <string>
#include <vector>
#include <filesystem>

/**
 * JarWriter - packages .class files into a JAR archive
 *
 * Uses the system `jar` command to create the JAR file.
 * Generates MANIFEST.MF with Main-Class and Class-Path entries.
 */
class JarWriter {
private:
    std::string outputDir;      // Directory containing .class files
    std::string jarPath;        // Path to output JAR file
    std::string mainClass;      // Main-Class for manifest
    std::string rtlJarPath;     // Path to RTL jar (for Class-Path)
    std::string manifestPath;   // Path to generated MANIFEST.MF

    /**
     * Write MANIFEST.MF file
     */
    bool writeManifest();

    /**
     * Build the jar command string
     */
    std::string buildJarCommand() const;

    /**
     * Find jar executable
     */
    std::string findJarExecutable() const;

public:
    /**
     * Create a JarWriter
     * @param outputDir Directory containing .class files
     * @param jarPath Path for output JAR file
     */
    JarWriter(const std::string& outputDir, const std::string& jarPath);

    /**
     * Set the main class for the JAR manifest
     * @param className Fully qualified class name (e.g., "com.example.Main")
     */
    void setMainClass(const std::string& className);

    /**
     * Set the path to RTL jar for Class-Path
     * @param path Path to rtl.jar
     */
    void setRtlJar(const std::string& path);

    /**
     * Create the JAR file
     * @return true if successful
     */
    bool write();

    /**
     * Get list of .class files in output directory
     */
    std::vector<std::string> getClassFiles() const;

    /**
     * Get the path to the created JAR
     */
    const std::string& getJarPath() const { return jarPath; }
};

#endif // SCALA_LEXER_JARWRITER_H
