cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 17)

FILE(GLOB_RECURSE RAVEN_HEADERS FOLLOW_SYMLINKS *.h)
FILE(GLOB_RECURSE RAVEN_CPPS FOLLOW_SYMLINKS *.cpp)

file(GLOB_RECURSE REMOVE_CMAKE "CMakeFiles/*")
list(REMOVE_ITEM RAVEN_CPPS ${REMOVE_CMAKE})


find_package(ZLIB REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)
if(NOT WIN32)
	find_library(DLPATH NAMES libdl.so dl REQUIRED)
endif()
 
set(LibSources ${RAVEN_HEADERS} ${RAVEN_CPPS})

add_library(Raven_CppClient OBJECT  ${LibSources})
 
set_property(TARGET Raven_CppClient PROPERTY POSITION_INDEPENDENT_CODE 1)

# shared and static libraries built from the same object files
add_library(Raven_CppClient_shared SHARED $<TARGET_OBJECTS:Raven_CppClient>)
add_library(Raven_CppClient_static STATIC $<TARGET_OBJECTS:Raven_CppClient>)

set_target_properties(Raven_CppClient_shared PROPERTIES OUTPUT_NAME Raven.CppClient.Shared) 
set_target_properties(Raven_CppClient_static PROPERTIES OUTPUT_NAME Raven.CppClient) 
 
if(WIN32)
	target_link_libraries(Raven_CppClient ZLIB::ZLIB nlohmann_json nlohmann_json::nlohmann_json)
else()
	target_link_libraries(Raven_CppClient ZLIB::ZLIB nlohmann_json nlohmann_json::nlohmann_json dl)
endif()

target_link_libraries(Raven_CppClient_shared PRIVATE Raven_CppClient)
target_link_libraries(Raven_CppClient_static PRIVATE Raven_CppClient)

 if(NOT CURL_LIBRARIES STREQUAL "")
	message("Found Curl libaries at ${CURL_LIBRARIES}")
 endif()

 if(NOT CURL_INCLUDE_DIRS STREQUAL "")
	message("Found Curl includes at ${CURL_INCLUDE_DIRS}")
endif()

message("Raven Client include paths: ${CMAKE_INCLUDE_PATH}")
set(CLIENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "RavenDB Client Directory")
message("Raven Client Dir: ${CLIENT_DIR}")
target_include_directories(Raven_CppClient PRIVATE 
	${ROOT_INCLUDE_PATH}/nlohmann
	${xxhash_cpp_SOURCE_DIR}/xxhash
    ${CMAKE_INCLUDE_PATH}
    ${CURL_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR})

if(WIN32)
	target_link_libraries(Raven_CppClient "Ws2_32.lib")
	target_link_libraries(Raven_CppClient "Wldap32.lib")
	target_link_libraries(Raven_CppClient "Crypt32.lib")
	target_link_libraries(Raven_CppClient "Rpcrt4.lib")	
endif()

target_link_libraries(Raven_CppClient OpenSSL::SSL OpenSSL::Crypto CURL::libcurl xxhash)

target_compile_definitions(Raven_CppClient_shared PUBLIC DEFAULT_CA_BUNDLE_PATH="../../ca_bundle/ca-bundle.crt")
target_compile_definitions(Raven_CppClient_static PUBLIC DEFAULT_CA_BUNDLE_PATH="../../ca_bundle/ca-bundle.crt")

install(TARGETS Raven_CppClient_static ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(TARGETS Raven_CppClient_shared LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
