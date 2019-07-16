cmake_minimum_required(VERSION 3.13)
project(Raven_CppClient_Tests)

add_subdirectory(lib/googletest/googletest)

FILE(GLOB RAVEN_TEST_HEADERS ./*.h)
FILE(GLOB RAVEN_TEST_CPPS ./*.cpp)

add_executable(Raven_CppClient_Tests ${RAVEN_TEST_HEADERS} ${RAVEN_TEST_CPPS})

target_include_directories(Raven_CppClient_Tests PRIVATE . .. ../Raven.CppClient ../external)

target_link_libraries(Raven_CppClient_Tests Raven_CppClient)
target_link_libraries(Raven_CppClient_Tests stdc++fs)
target_link_libraries(Raven_CppClient_Tests gtest gtest_main)
target_link_libraries(Raven_CppClient_Tests curl ssl crypto pthread)
