project(${PROJECT_NAME}__how-many-numbers-are-smaller-than-the-current-number)

file(GLOB_RECURSE SOURCE_FILES *.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
