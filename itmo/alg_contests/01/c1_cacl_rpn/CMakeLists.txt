project(${PROJECT_NAME}__c1_cacl_rpn)

file(GLOB_RECURSE SOURCE_FILES *.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
