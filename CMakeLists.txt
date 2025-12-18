cmake_minimum_required(VERSION 3.20)
project(scala_lexer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(FLEX_EXECUTABLE "D:/soft/win_flex_bison/win_flex.exe")
set(BISON_EXECUTABLE "D:/soft/win_flex_bison/win_bison.exe")

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

#BISON_TARGET(MyParser scala_parser.y ${CMAKE_CURRENT_BINARY_DIR}/scala_parser.cpp
#        COMPILE_FLAGS "-v -Wcounterexamples -Wno-conflicts-sr --report=solved --report-file=${CMAKE_CURRENT_BINARY_DIR}/bison.report"
#)
BISON_TARGET(MyParser scala_parser.y ${CMAKE_CURRENT_BINARY_DIR}/scala_parser.cpp
        COMPILE_FLAGS "-v --report=solved --report-file=${CMAKE_CURRENT_BINARY_DIR}/bison.report"
)
FLEX_TARGET(MyScanner scala_lexer.l ${CMAKE_CURRENT_BINARY_DIR}/scala_lexer.cpp)

ADD_FLEX_BISON_DEPENDENCY(MyScanner MyParser)

add_executable(scala_lexer
        ${FLEX_MyScanner_OUTPUTS}
        ${BISON_MyParser_OUTPUTS}
)

message(STATUS "Flex output: ${FLEX_MyScanner_OUTPUTS}")
message(STATUS "Bison output: ${BISON_MyParser_OUTPUTS}")

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