project(${PROJECT_NAME}__hw2_task4)

file(GLOB_RECURSE SOURCE_FILES *.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} GTest::gtest_main)
gtest_discover_tests(${PROJECT_NAME})
