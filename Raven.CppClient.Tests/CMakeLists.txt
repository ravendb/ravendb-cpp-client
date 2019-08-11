cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 17)

FILE(GLOB_RECURSE RAVEN_TEST_HEADERS *.h)
FILE(GLOB_RECURSE RAVEN_TEST_CPPS *.cpp)

if(MSVC)
	add_compile_options(/bigobj)
endif()

IF(NOT WIN32)
    set(CMAKE_THREAD_LIBS_INIT "-lpthread")
    set(CMAKE_HAVE_THREADS_LIBRARY 1)
    set(CMAKE_USE_WIN32_THREADS_INIT 0)
    set(CMAKE_USE_PTHREADS_INIT 1)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
ENDIF()

add_executable(Raven_CppClient_Tests ${RAVEN_TEST_HEADERS} ${RAVEN_TEST_CPPS})

#login files
if(WIN32)
	file(COPY "${PROJECT_SOURCE_DIR}/win_re_config/secured_re.txt" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/")
	file(COPY "${PROJECT_SOURCE_DIR}/win_re_config/unsecured_re.txt" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/")
else()
	file(COPY "${PROJECT_SOURCE_DIR}/linux_re_config/secured_re.txt" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/")
	file(COPY "${PROJECT_SOURCE_DIR}/linux_re_config/unsecured_re.txt" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/")
endif()

find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)
find_package(Threads REQUIRED)
find_package(GTest CONFIG REQUIRED)

message("CLIENT_DIR: ${CLIENT_DIR}")

target_include_directories(Raven_CppClient_Tests PRIVATE	
	${CLIENT_DIR}
	${ROOT_INCLUDE_PATH}/nlohmann
    ${CMAKE_INCLUDE_PATH}
    ${gtest_SOURCE_DIR}/include 
    ${gtest_SOURCE_DIR} 
    ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Raven_CppClient_Tests PRIVATE 
    GTest::gtest 
    GTest::gtest_main 
    Raven_CppClient 
    OpenSSL::SSL 
    OpenSSL::Crypto 
    ${CURL_LIBRARIES} 
    Threads::Threads)

target_link_directories(Raven_CppClient_Tests PRIVATE ${CMAKE_LIBRARY_PATH})

if(NOT WIN32)
	target_link_libraries(Raven_CppClient_Tests PRIVATE stdc++fs)
endif()

include(GoogleTest)
gtest_discover_tests(Raven_CppClient_Tests)  

if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/secured_re.txt)
	message("Copying config file secured_re.txt from ${CMAKE_SOURCE_DIR}/${CONFIG_DIR}/ to ${CMAKE_CURRENT_BINARY_DIR}/")
	add_custom_command(                                                                                                                                                                                                                                                                    TARGET Raven_CppClient_Tests POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy
				${CMAKE_SOURCE_DIR}/${CONFIG_DIR}/secured_re.txt
				${CMAKE_CURRENT_BINARY_DIR}/secured_re.txt)
else()
	message("Using config file secured_re.txt at ${CMAKE_CURRENT_BINARY_DIR}/")
endif()

if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/unsecured_re.txt)	
	message("Copying config file unsecured_re.txt from ${CMAKE_SOURCE_DIR}/${CONFIG_DIR}/ to ${CMAKE_CURRENT_BINARY_DIR}/")
	add_custom_command(
			TARGET Raven_CppClient_Tests POST_BUILD
					COMMAND ${CMAKE_COMMAND} -E copy
						${CMAKE_SOURCE_DIR}/${CONFIG_DIR}/unsecured_re.txt
						${CMAKE_CURRENT_BINARY_DIR}/unsecured_re.txt)
else()
	message("Using config file unsecured_re.txt at ${CMAKE_CURRENT_BINARY_DIR}/")
endif()    