cmake_minimum_required(VERSION 3.13)
project(RavenDBCppClient)
include(CTest) 

set(CMAKE_CXX_STANDARD 17)
 
if(MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG "/MTd /Zi /Ob0 /Od /RTC1")
	set(CMAKE_CXX_FLAGS_RELEASE "/MT /O2 /Ob2 /DNDEBUG")	
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
 
#library names for vcpkg to install
if(WIN32)
	set(PROJECT_DEPENDENCIES curl nlohmann-json openssl gtest zlib pthreads)
else()
	set(PROJECT_DEPENDENCIES curl nlohmann-json openssl gtest zlib pthreads libuuid)
endif()
 
include(cmake/vcpkg_config.cmake)
include(cmake/xxhash.cmake)
    
message(STATUS "xxhash_cpp include dir: ${xxhash_cpp_SOURCE_DIR}/xxhash")

list(APPEND CMAKE_INCLUDE_PATH "${xxhash_cpp_SOURCE_DIR}/xxhash")
 
add_subdirectory(Raven.CppClient)
 
#choose if buildings tryouts
option(BUILD_TRYOUTS "Build the Tryouts" OFF)
if(BUILD_TRYOUTS)
    add_subdirectory(Tryouts) 
endif()
  
#choose if building tests
option(BUILD_TESTS "Build the Tests" ON)
if(BUILD_TESTS)
	enable_testing()
	if(WIN32)
		set(CONFIG_DIR "${PROJECT_SOURCE_DIR}/win_re_config" CACHE INTERNAL "")
	else()
		set(CONFIG_DIR "${PROJECT_SOURCE_DIR}/linux_re_config" CACHE INTERNAL "")
	endif()

	message("Test config folder: ${PROJECT_SOURCE_DIR}/win_re_config")

    add_subdirectory(Raven.CppClient.Tests)
endif()

#set user home directory for installation
if(WINN32)
    set(USER_HOME_DIRECTORY $ENV{HOMEPATH})
endif()
if(UNIX)
    set(USER_HOME_DIRECTORY $ENV{HOME})
endif()


if(CMAKE_INSTALL_PREFIX EQUAL "")
    set(CMAKE_INSTALL_PREFIX ${USER_HOME_DIRECTORY}/RavenDBCppClient CACHE PATH "destination directory" FORCE)
endif()

#FILE(GLOB include_files Raven.CppClient/*.h)
#FILE(GLOB external_include_files external/*)
#FILE(GLOB ca_bundle_files ca_bundle/*)

#FILE(GLOB HELLO_WORLD_APP ./hello_world.cpp)

#install(TARGETS Raven_CppClient LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
#install(FILES ${include_files} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
#install(FILES ${external_include_files} DESTINATION ${CMAKE_INSTALL_PREFIX}/external)
#install(FILES ${ca_bundle_files} DESTINATION ${CMAKE_INSTALL_PREFIX}/ca_bundle)
#install(FILES ${HELLO_WORLD_APP} DESTINATION ${CMAKE_INSTALL_PREFIX}/)
