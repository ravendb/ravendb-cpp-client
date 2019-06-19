cmake_minimum_required(VERSION 3.13)
project(raven_cpp_client)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(Raven.CppClient)
add_subdirectory(Tryouts)
add_subdirectory(Raven.CppClient.Tests)


set(CMAKE_INSTALL_PREFIX ~/RavenDBCppClient)

FILE(GLOB include_files Raven.CppClient/*.h)
FILE(GLOB external_include_files external/*)
FILE(GLOB ca_bundle_files ca_bundle/*)

install(TARGETS Raven_CppClient LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES ${include_files} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
install(FILES ${external_include_files} DESTINATION ${CMAKE_INSTALL_PREFIX}/external)
install(FILES ${ca_bundle_files} DESTINATION ${CMAKE_INSTALL_PREFIX}/ca_bundle)



