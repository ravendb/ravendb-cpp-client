project(Raven_CppClient_Tests)

add_subdirectory(lib/googletest/googletest)

add_executable(Raven_CppClient_Tests
        Address.h
        AdvancedPatchingTests.cpp
        Arr.h
        BasicDocumentCommandsTests.cpp
        BasicDocumentsSessionTests.cpp
        BasicPatchTests.cpp
        BasicQueryTests.cpp
        BoundingBoxIndexTest.cpp
        ClientConfigurationTest.cpp
        Company.h
        CompanyType.h
        ConnectionDetailsHolder.cpp
        ConnectionDetailsHolder.h
        Contact.h
        CreateSampleDataOperation.cpp
        CreateSampleDataOperation.h
        CrudTest.cpp
        DeleteDocumentCommandTest.cpp
        DeleteTest.cpp
        DocumentIdPropertyTest.cpp
        ds_definitions.cpp
        ds_definitions.h
        Employee.h
        ExistsTest.cpp
        Family.h
        FirstClassPatchTest.cpp
        GeekPerson.h
        GetDocumentsCommandTests.cpp
        GetNextOperationIdCommandTest.cpp
        GetStatisticsCommandTest.cpp
        HiLoTest.cpp
        HttpsTest.cpp
        IndexesFromClientTest.cpp
        IndexOperationsTest.cpp
        IndexOperationsTestsOld.cpp
        JavaScriptIndexTest.cpp
        LazyTest.cpp
        LoadAllStartingWith.cpp
        LoadTest.cpp
        LoadTests.cpp
        main.cpp
        NullableUser.h
        Order.h
        OrderLine.h
        pch.cpp
        pch.h
        Person.h
        Poc.h
        Post.h
        PutDocumentCommandTest.cpp
        QueryTest.cpp
        raven_test_definitions.cpp
        raven_test_definitions.h
        RavenDB_8761.cpp
        RavenTestDriver.cpp
        RavenTestDriver.h
        re_definitions.cpp
        re_definitions.h
        RequestExecutorAndDBsTests.cpp
        RequestExecutorTest.cpp
        SecuredRequestExecutorTests.cpp
        SimpleMultiMapTest.cpp
        test_utils.cpp
        test_utils.h
        TrackEntityTest.cpp
        User.h
        WhatChangedTest.cpp)

target_include_directories(Raven_CppClient_Tests PRIVATE . .. ../Raven.CppClient)

find_library(RAVEN_CPPCLIENT Raven_CppClient ../cmake-build-debug/Raven.CppClient)

target_link_libraries(Raven_CppClient_Tests stdc++fs)
target_link_libraries(Raven_CppClient_Tests gtest gtest_main)
target_link_libraries(Raven_CppClient_Tests curl ssl crypto pthread)
target_link_libraries(Raven_CppClient_Tests ${RAVEN_CPPCLIENT})