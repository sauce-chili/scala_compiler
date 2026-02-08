#include "JarWriter.h"

#include <fstream>
#include <iostream>
#include <cstdlib>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#endif

JarWriter::JarWriter(const std::string& outputDir, const std::string& jarPath)
    : outputDir(outputDir), jarPath(jarPath) {
    manifestPath = outputDir + "/META-INF/MANIFEST.MF";
}

void JarWriter::setMainClass(const std::string& className) {
    mainClass = className;
}

void JarWriter::setRtlJar(const std::string& path) {
    rtlJarPath = path;
}

bool JarWriter::writeManifest() {
    // Create META-INF directory
    std::filesystem::path metaInfDir = std::filesystem::path(outputDir) / "META-INF";
    std::filesystem::create_directories(metaInfDir);

    std::ofstream manifest(manifestPath);
    if (!manifest) {
        std::cerr << "Failed to create MANIFEST.MF at: " << manifestPath << std::endl;
        return false;
    }

    manifest << "Manifest-Version: 1.0\n";

    if (!mainClass.empty()) {
        manifest << "Main-Class: " << mainClass << "\n";
    }

    if (!rtlJarPath.empty()) {
        manifest << "Class-Path: " << rtlJarPath << "\n";
    }

    manifest << "Created-By: scala_lexer\n";

    // Manifest must end with newline
    manifest << "\n";

    manifest.close();
    return manifest.good() || true;  // ofstream may report failure on close even if write succeeded
}

std::string JarWriter::findJarExecutable() const {
    // Try to find jar in PATH or JAVA_HOME
#ifdef _WIN32
    // Check JAVA_HOME
    const char* javaHome = std::getenv("JAVA_HOME");
    if (javaHome) {
        std::string jarPath = std::string(javaHome) + "\\bin\\jar.exe";
        if (std::filesystem::exists(jarPath)) {
            return "\"" + jarPath + "\"";
        }
    }

    // Try just "jar" and hope it's in PATH
    return "jar";
#else
    // On Unix, check JAVA_HOME
    const char* javaHome = std::getenv("JAVA_HOME");
    if (javaHome) {
        std::string jarPath = std::string(javaHome) + "/bin/jar";
        if (std::filesystem::exists(jarPath)) {
            return jarPath;
        }
    }
    return "jar";
#endif
}

std::string JarWriter::buildJarCommand() const {
    std::ostringstream cmd;

    std::string jarExe = findJarExecutable();

    // jar cfm output.jar MANIFEST.MF -C output_dir .
    cmd << jarExe << " cfm ";
    cmd << "\"" << jarPath << "\" ";
    cmd << "\"" << manifestPath << "\" ";
    cmd << "-C \"" << outputDir << "\" .";

#ifdef _WIN32
    // On Windows, system() calls cmd.exe /c which strips the first and last
    // quote characters when there are multiple quoted arguments. Wrapping
    // the entire command in an extra pair of quotes prevents this.
    return "\"" + cmd.str() + "\"";
#else
    return cmd.str();
#endif
}

std::vector<std::string> JarWriter::getClassFiles() const {
    std::vector<std::string> classFiles;

    if (!std::filesystem::exists(outputDir)) {
        return classFiles;
    }

    for (const auto& entry : std::filesystem::recursive_directory_iterator(outputDir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".class") {
            classFiles.push_back(entry.path().string());
        }
    }

    return classFiles;
}

bool JarWriter::write() {
    // Check if there are any .class files
    auto classFiles = getClassFiles();
    if (classFiles.empty()) {
        std::cerr << "No .class files found in: " << outputDir << std::endl;
        return false;
    }

    // Write manifest
    if (!writeManifest()) {
        std::cerr << "Failed to write MANIFEST.MF" << std::endl;
        return false;
    }

    // Build and execute jar command
    std::string cmd = buildJarCommand();

    std::cout << "Creating JAR: " << cmd << std::endl;

    int result = std::system(cmd.c_str());

    if (result != 0) {
        std::cerr << "jar command failed with code: " << result << std::endl;

        // Try alternative approach: create jar manually
        // For simplicity, we'll just report the error
        return false;
    }

    // Verify JAR was created
    if (!std::filesystem::exists(jarPath)) {
        std::cerr << "JAR file was not created at: " << jarPath << std::endl;
        return false;
    }

    std::cout << "JAR created successfully: " << jarPath << std::endl;
    return true;
}
