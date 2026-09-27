cmake_minimum_required(VERSION 3.22)

project(p LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

include(install_googletest.cmake)
include(add_project_with_subdirs.cmake)

include_directories(utility)

add_subdirectory(leetcode)
add_subdirectory(itmo)
