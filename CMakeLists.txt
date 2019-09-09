cmake_minimum_required(VERSION 3.0)
project (spectre)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE CODEGEN_SOURCES "codegen/*.cpp")
file(GLOB_RECURSE CORE_SOURCES "core/*.cpp")
file(GLOB_RECURSE IR_SOURCES "ir/*.cpp")
file(GLOB_RECURSE LEXER_SOURCES "lexer/*.cpp")
file(GLOB_RECURSE OPT_SOURCES "opt/*.cpp")
file(GLOB_RECURSE PARSER_SOURCES "parser/*.cpp")
file(GLOB_RECURSE PASS_SOURCES "pass/*.cpp")
file(GLOB MAIN_SOURCES "*.cpp")
set(SOURCES ${CODEGEN_SOURCES} ${CORE_SOURCES} ${IR_SOURCES} ${LEXER_SOURCES} ${OPT_SOURCES}
	${PARSER_SOURCES} ${PASS_SOURCES} ${MAIN_SOURCES})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++17 -Wno-psabi")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

add_executable(spectre ${SOURCES})
target_link_libraries(${PROJECT_NAME} stdc++fs)
