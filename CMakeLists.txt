cmake_minimum_required(VERSION 3.20)
project(scala_lexer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Укажите свои пути, если они отличаются
set(FLEX_EXECUTABLE "D:/soft/win_flex_bison/win_flex.exe")
set(BISON_EXECUTABLE "D:/soft/win_flex_bison/win_bison.exe")

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

# Включаем текущую директорию и build-директорию (для сгенерированных хедеров)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Автоматически находим все исходники узлов и вспомогательных классов лексера
file(GLOB_RECURSE NODE_SOURCES "nodes/*.cpp")
file(GLOB_RECURSE LEXER_UTILS_SOURCES "lexer/*.cpp")
file(GLOB_RECURSE SEMANTIC "semantic/*cpp")

# Настройка BISON
# Добавлен флаг -d (или --defines), чтобы генерировать заголовочный файл .h
BISON_TARGET(MyParser scala_parser.y ${CMAKE_CURRENT_BINARY_DIR}/scala_parser.cpp
        COMPILE_FLAGS "-v -d --report=solved --report-file=${CMAKE_CURRENT_BINARY_DIR}/bison.report"
)
#BISON_TARGET(MyParser scala_parser.y ${CMAKE_CURRENT_BINARY_DIR}/scala_parser.cpp
#        COMPILE_FLAGS "-v -Wcounterexamples -Wno-conflicts-sr --report=solved --report-file=${CMAKE_CURRENT_BINARY_DIR}/bison.report"
#)

# Настройка FLEX
FLEX_TARGET(MyScanner scala_lexer.l ${CMAKE_CURRENT_BINARY_DIR}/scala_lexer.cpp)

# Указываем, что лексер зависит от заголовков парсера
ADD_FLEX_BISON_DEPENDENCY(MyScanner MyParser)

# Собираем исполняемый файл
add_executable(scala_lexer
        main.cpp
        ${BISON_MyParser_OUTPUTS}
        ${FLEX_MyScanner_OUTPUTS}
        ${NODE_SOURCES}
        ${LEXER_UTILS_SOURCES}
        ${SEMANTIC}
        semantic/Semantic.cpp
        code_generation/tools/byte_convert.cpp
        nodes/class/SuperConstructorCallNode.cpp
        nodes/class/SuperConstructorCallNode.h
        nodes/class/PrimaryConstructorNode.cpp
        nodes/class/PrimaryConstructorNode.h
)

message(STATUS "Flex output: ${FLEX_MyScanner_OUTPUTS}")
message(STATUS "Bison output: ${BISON_MyParser_OUTPUTS}")

# Подавление предупреждений компилятора для сгенерированных файлов
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set_source_files_properties(
            ${FLEX_MyScanner_OUTPUTS}
            ${BISON_MyParser_OUTPUTS}
            PROPERTIES COMPILE_OPTIONS "-Wno-register;-Wno-deprecated;-Wno-unused-function"
    )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set_source_files_properties(
            ${FLEX_MyScanner_OUTPUTS}
            ${BISON_MyParser_OUTPUTS}
            PROPERTIES COMPILE_OPTIONS "/wd5033;/wd4996"
    )
endif ()