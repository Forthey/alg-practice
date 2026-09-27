project(${PROJECT_NAME}__b1_redundant_whitespaces)

file(GLOB_RECURSE SOURCE_FILES *.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
