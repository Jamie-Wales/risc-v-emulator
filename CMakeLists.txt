cmake_minimum_required(VERSION 3.14)
project(risc-emu)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE MAIN_SOURCE
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

add_executable(risc-emu ${MAIN_SOURCE})
target_include_directories(risc-emu PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

set_target_properties(risc-emu PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
