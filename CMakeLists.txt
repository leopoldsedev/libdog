cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME libdog)
set(TEST_NAME ${PROJECT_NAME}_test)

project(${PROJECT_NAME} VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#set(CMAKE_VERBOSE_MAKEFILE on)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG -O2")
add_compile_options("-Wall")
add_compile_options("-Wextra")


set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

set(SRC_EXT_PATTERN ".cpp")
file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*${SRC_EXT_PATTERN})

include_directories(include)

add_library(${PROJECT_NAME} ${SRC_FILES})

add_subdirectory(demo)


enable_testing()

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

file(GLOB TEST_SRC_FILES ${PROJECT_SOURCE_DIR}/test/*${SRC_EXT_PATTERN})
add_executable(${TEST_NAME} ${TEST_SRC_FILES})

target_link_libraries(${TEST_NAME} ${GTEST_BOTH_LIBRARIES} ${PROJECT_NAME})

include(GoogleTest)
gtest_discover_tests(${TEST_NAME})
