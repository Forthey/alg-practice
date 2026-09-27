project(${PROJECT_NAME}__max_consecutive_ones)

file(GLOB_RECURSE SOURCE_FILES *.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
