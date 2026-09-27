include_guard(GLOBAL)

# The Rust manager validates declarations and file layout before generation.
function(add_task)
    cmake_parse_arguments(TASK "" "NAME;DIRECTORY;INCLUDE_TESTS" "SOURCES" ${ARGN})
    set(task_dir "${PROJECT_SOURCE_DIR}/${TASK_DIRECTORY}")
    if(TASK_INCLUDE_TESTS)
        set(entry "tests.cpp")
        if(NOT TARGET GTest::gtest_main)
            include("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../install_googletest.cmake")
        endif()
        include(GoogleTest)
    else()
        set(entry "main.cpp")
    endif()

    set(sources "${task_dir}/${entry}")
    foreach(source IN LISTS TASK_SOURCES)
        list(APPEND sources "${task_dir}/${source}")
    endforeach()
    add_executable(${TASK_NAME} ${sources})
    target_include_directories(${TASK_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/utility" "${task_dir}")
    if(MSVC)
        target_compile_options(${TASK_NAME} PRIVATE /W4 /WX /utf-8)
    else()
        target_compile_options(${TASK_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()

    if(TASK_INCLUDE_TESTS)
        target_link_libraries(${TASK_NAME} PRIVATE GTest::gtest_main)
        gtest_discover_tests(${TASK_NAME}
            TEST_PREFIX "${TASK_NAME}."
            NO_PRETTY_VALUES
            PROPERTIES LABELS "${TASK_DIRECTORY}")
    endif()
endfunction()
