cmake_minimum_required(VERSION 3.20)
project(untitled C)
link_libraries(crypto)
set(CMAKE_C_STANDARD 99)

add_executable(untitled main.c encrypt.c decrypt.c tests.c encrypt.h decrypt.h)
