cmake_minimum_required(VERSION 3.13)
project(Raven_CppClient)

set(CMAKE_CXX_STANDARD 17)

FILE(GLOB RAVEN_HEADERS ./*.h)
FILE(GLOB RAVEN_CPPS ./*.cpp)

add_library(Raven_CppClient SHARED ${RAVEN_HEADERS} ${RAVEN_CPPS})


target_include_directories(Raven_CppClient PUBLIC . .. ../external)

target_compile_definitions(Raven_CppClient PUBLIC DEFAULT_CA_BUNDLE_PATH="../../ca_bundle/ca-bundle.crt")

add_custom_command(TARGET Raven_CppClient PRE_BUILD COMMAND bash ./prepare_one_header.sh
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

add_executable(PrepareOneHeader prepare_one_header.cpp)
target_link_libraries(PrepareOneHeader stdc++fs)


