cmake_minimum_required(VERSION 3.20)
project(scala_lexer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(FLEX REQUIRED)

FLEX_TARGET(MyScanner scala_lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)

add_executable(lexer
        ${FLEX_MyScanner_OUTPUTS}
        scala_lexer.l
)

# Отключаем предупреждения про "register" только для lexer.cpp
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set_source_files_properties(${FLEX_MyScanner_OUTPUTS}
            PROPERTIES COMPILE_OPTIONS "-Wno-register")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set_source_files_properties(${FLEX_MyScanner_OUTPUTS}
            PROPERTIES COMPILE_OPTIONS "/wd5033")
endif()
