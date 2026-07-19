cmake_minimum_required(VERSION 3.22)

project(practice LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

include_directories(utility)

add_subdirectory(easy)
add_subdirectory(medium)
add_subdirectory(hard)
