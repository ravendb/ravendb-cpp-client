cmake_minimum_required(VERSION 3.14)
project(Raven_CppClient_Tryouts)

set(CMAKE_CXX_STANDARD 17)

add_executable(Raven_CppClient_Tryouts
        pch.cpp
        pch.h
        resource.h
        Tryouts.cpp
        #tryouts1.cpp
        #tryouts2.cpp
        )

target_include_directories(Raven_CppClient_Tryouts PRIVATE . ../external ../Raven.CppClient ../Raven.CppClient.Tests)

target_link_libraries(Raven_CppClient_Tryouts Raven_CppClient)
target_link_libraries(Raven_CppClient_Tryouts stdc++fs)
target_link_libraries(Raven_CppClient_Tryouts curl ssl crypto pthread)