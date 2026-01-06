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
        nodes/Node.cpp
        nodes/Node.h
        nodes/Types.cpp
        nodes/Types.h
        nodes/modifier/ModifierNode.cpp
        nodes/modifier/ModifierNode.h
        nodes/modifier/ModifiersNode.cpp
        nodes/modifier/ModifiersNode.h
        nodes/id/IdNode.cpp
        nodes/id/IdNode.h
        nodes/exprs/SimpleExpr1Node.cpp
        nodes/exprs/SimpleExpr1Node.h
        nodes/exprs/ArgumentExprsNode.cpp
        nodes/exprs/ArgumentExprsNode.h
        nodes/exprs/SimpleExprNode.cpp
        nodes/exprs/SimpleExprNode.h
        nodes/exprs/ExprNode.cpp
        nodes/exprs/ExprNode.h
        nodes/exprs/ExprsNode.cpp
        nodes/exprs/ExprsNode.h
        nodes/class/ConstrInvokeNode.cpp
        nodes/class/ConstrInvokeNode.h
        nodes/stats/BlockStatsNode.cpp
        nodes/stats/BlockStatsNode.h
        nodes/type/SimpleTypeNode.cpp
        nodes/type/SimpleTypeNode.h
        nodes/id/StableIdNode.cpp
        nodes/id/StableIdNode.h
        nodes/type/CompoundTypeNode.cpp
        nodes/type/CompoundTypeNode.h
        nodes/id/SingleStableIdNode.cpp
        nodes/id/SingleStableIdNode.h
        nodes/stats/BlockStatNode.cpp
        nodes/stats/BlockStatNode.h
        nodes/var/VarDefsNode.cpp
        nodes/var/VarDefsNode.h
        nodes/id/IdsNode.cpp
        nodes/id/IdsNode.h
        nodes/try/TryExprNode.cpp
        nodes/try/TryExprNode.h
        nodes/exprs/PrefixExprNode.cpp
        nodes/exprs/PrefixExprNode.h
        nodes/stats/EnumStats.cpp
        nodes/stats/EnumStats.h
        nodes/stats/EnumStat.cpp
        nodes/stats/EnumStat.h
        nodes/exprs/AssignExprNode.cpp
        nodes/exprs/AssignExprNode.h
        nodes/class/ClassParamNode.cpp
        nodes/class/ClassParamNode.h
        nodes/class/ClassParamsNode.cpp
        nodes/class/ClassParamsNode.h
        nodes/func/FuncParamNode.cpp
        nodes/func/FuncParamNode.h
        nodes/func/FuncParamsNode.cpp
        nodes/func/FuncParamsNode.h
        nodes/class/ClassParentsNode.cpp
        nodes/class/ClassParentsNode.h
        nodes/type/SimpleTypesNode.cpp
        nodes/type/SimpleTypesNode.h
        nodes/enum/EnumCaseNode.cpp
        nodes/enum/EnumCaseNode.h
        nodes/stats/TemplateStatNode.cpp
        nodes/stats/TemplateStatNode.h
        nodes/func/FunSigNode.cpp
        nodes/func/FunSigNode.h
        nodes/definitions/DclNode.cpp
        nodes/definitions/DclNode.h
        nodes/func/FunDefNode.cpp
        nodes/func/FunDefNode.h
        nodes/exprs/ConstrExprNode.cpp
        nodes/exprs/ConstrExprNode.h
        nodes/templates/TemplateStatNode.cpp
        nodes/templates/TemplateStatNode.h
        nodes/definitions/DefNode.cpp
        nodes/definitions/DefNode.h
        nodes/templates/TemplateStatsNode.cpp
        nodes/templates/TemplateStatsNode.h
        nodes/class/ClassTemplateNode.cpp
        nodes/class/ClassTemplateNode.h
        nodes/templates/ClassTemplateOptNode.cpp
        nodes/templates/ClassTemplateOptNode.h
        nodes/class/ClassDefNode.cpp
        nodes/class/ClassDefNode.h
        nodes/enum/EnumStatNode.cpp
        nodes/enum/EnumStatNode.h
        nodes/enum/EnumStatsNode.cpp
        nodes/enum/EnumStatsNode.h
        nodes/templates/EnumTemplateNode.cpp
        nodes/templates/EnumTemplateNode.h
        nodes/enum/EnumDefNode.cpp
        nodes/enum/EnumDefNode.h
        nodes/trait/TraitTemplateNode.cpp
        nodes/trait/TraitTemplateNode.h
        nodes/trait/TraitTemplateOptNode.cpp
        nodes/trait/TraitTemplateOptNode.h
        nodes/templates/TemplateDefNode.cpp
        nodes/templates/TemplateDefNode.h
        nodes/stats/TopStatNode.cpp
        nodes/stats/TopStatNode.h
        nodes/stats/TopStatSeqNode.cpp
        nodes/stats/TopStatSeqNode.h
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