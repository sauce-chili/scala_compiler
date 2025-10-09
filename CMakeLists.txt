cmake_minimum_required(VERSION 3.20)
project(scala_lexer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# если win_bison, то переименовать в bison.exe
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

BISON_TARGET(MyParser scala_parser.y ${CMAKE_CURRENT_BINARY_DIR}/scala_parser.cpp)
FLEX_TARGET(MyScanner scala_lexer.l ${CMAKE_CURRENT_BINARY_DIR}/scala_lexer.cpp)

add_executable(scala_lexer
        main.cpp
        ${BISON_MyParser_OUTPUTS}
        ${FLEX_MyScanner_OUTPUTS}
)

set_source_files_properties(${FLEX_MyScanner_OUTPUTS}
        PROPERTIES OBJECT_DEPENDS ${BISON_MyParser_HEADER}
)

# FlexLexer.h обычно не нужен явно, но на всякий:
# (если будет ошибка — раскомментируй)
# target_include_directories(scala_lexer PRIVATE "D:/soft/win_flex_bison")

# Отключаем предупреждения про "register" только для lexer.cpp
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set_source_files_properties(${FLEX_MyScanner_OUTPUTS}
            PROPERTIES COMPILE_OPTIONS "-Wno-register")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set_source_files_properties(${FLEX_MyScanner_OUTPUTS}
            PROPERTIES COMPILE_OPTIONS "/wd5033")
endif()