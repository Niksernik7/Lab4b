cmake_minimum_required(VERSION 3.21)
project(untitled C)

set(CMAKE_C_STANDARD 99)
add_compile_definitions(DEBUG)

add_executable(untitled main.c RBTree.c Menu.c)
add_executable(tests tests.c RBTree.c)
