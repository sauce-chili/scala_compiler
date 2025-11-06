cmake_minimum_required(VERSION 3.20)
project(scala_lexer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск Flex и Bison
find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Генерация парсера через Bison
BISON_TARGET(MyParser scala_parser.y ${CMAKE_CURRENT_BINARY_DIR}/scala_parser.cpp
        COMPILE_FLAGS "-v"
)
# Генерация лексера через Flex (указываем заголовок Bison для токенов)
FLEX_TARGET(MyScanner scala_lexer.l ${CMAKE_CURRENT_BINARY_DIR}/scala_lexer.cpp)

# Объявляем зависимости между лексером и парсером
ADD_FLEX_BISON_DEPENDENCY(MyScanner MyParser)

# Создаем исполняемый файл
add_executable(scala_lexer
        ${FLEX_MyScanner_OUTPUTS}
        ${BISON_MyParser_OUTPUTS}
)

message(STATUS "Flex output: ${FLEX_MyScanner_OUTPUTS}")
message(STATUS "Bison output: ${BISON_MyParser_OUTPUTS}")

# Отключаем предупреждения для сгенерированных файлов
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set_source_files_properties(
            ${FLEX_MyScanner_OUTPUTS}
            ${BISON_MyParser_OUTPUTS}
            PROPERTIES COMPILE_OPTIONS "-Wno-register;-Wno-deprecated"
    )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set_source_files_properties(
            ${FLEX_MyScanner_OUTPUTS}
            ${BISON_MyParser_OUTPUTS}
            PROPERTIES COMPILE_OPTIONS "/wd5033"
    )
endif()