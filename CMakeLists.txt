cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(Collection_C LANGUAGES C)

include_directories("include")

add_executable(Programa "src/main.c")
target_include_directories(Programa PUBLIC "include")
