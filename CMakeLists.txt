cmake_minimum_required(VERSION 3.10.0)
project(vitemu2 VERSION 0.1.0 LANGUAGES C CXX)

add_executable(vitemu2 
    src/main.cpp
    src/cpu/cpu.cpp
    src/memory/memory.cpp
)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
