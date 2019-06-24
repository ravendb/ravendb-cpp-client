cmake_minimum_required(VERSION 3.13)
project(raven_cpp_client)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(Raven.CppClient)

#choose if buildings tryouts
option(BUILD_TRYOUTS "Build the tryouts" OFF)
if(BUILD_TRYOUTS)
    add_subdirectory(Tryouts)
endif()

#choose if building tests
option(BUILD_TESTS "Build the Tests" OFF)
if(BUILD_TESTS)
    add_subdirectory(Raven.CppClient.Tests)
endif()

#set user home directory for installation
if(WINN32)
    set(USER_HOME_DIRECTORY $ENV{HOMEPATH})
endif()
if(UNIX)
    set(USER_HOME_DIRECTORY $ENV{HOME})
endif()


if(NOT CMAKE_INSTALL_PREFIX EQUAL "")
    set(CMAKE_INSTALL_PREFIX ${USER_HOME_DIRECTORY}/RavenDBCppClient CACHE PATH "destination directory" FORCE)
endif()

FILE(GLOB include_files Raven.CppClient/*.h)
FILE(GLOB external_include_files external/*)
FILE(GLOB ca_bundle_files ca_bundle/*)

FILE(GLOB HELLO_WORLD_APP ./hello_world.cpp)

install(TARGETS Raven_CppClient LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES ${include_files} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
install(FILES ${external_include_files} DESTINATION ${CMAKE_INSTALL_PREFIX}/external)
install(FILES ${ca_bundle_files} DESTINATION ${CMAKE_INSTALL_PREFIX}/ca_bundle)

install(FILES ${HELLO_WORLD_APP} DESTINATION ${CMAKE_INSTALL_PREFIX}/)
