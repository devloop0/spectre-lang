cmake_minimum_required(VERSION 3.0)
project (spectre)

include_directories("opt_passes" ".")

file(GLOB SOURCES "*.cpp" "opt_passes/*.cpp")

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++17 -Wno-psabi")
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

add_executable(spectre ${SOURCES})
target_link_libraries(${PROJECT_NAME} stdc++fs)
