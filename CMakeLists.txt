cmake_minimum_required(VERSION 3.30)

project(algorithm_practice LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()
include(cmake/AddTask.cmake)
include(tasks.cmake)
