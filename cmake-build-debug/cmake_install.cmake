# Install script for directory: /home/alexander/RavenDB_Client/raven-cpp-client

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/alexander/RavenDBCppClient")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/home/alexander/RavenDBCppClient/lib/libRaven_CppClient.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/alexander/RavenDBCppClient/lib/libRaven_CppClient.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/alexander/RavenDBCppClient/lib/libRaven_CppClient.so"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/alexander/RavenDBCppClient/lib/libRaven_CppClient.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/alexander/RavenDBCppClient/lib" TYPE SHARED_LIBRARY FILES "/home/alexander/RavenDB_Client/raven-cpp-client/cmake-build-debug/Raven.CppClient/libRaven_CppClient.so")
  if(EXISTS "$ENV{DESTDIR}/home/alexander/RavenDBCppClient/lib/libRaven_CppClient.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/alexander/RavenDBCppClient/lib/libRaven_CppClient.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/alexander/RavenDBCppClient/lib/libRaven_CppClient.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/alexander/RavenDBCppClient/include/AbstractDocumentQuery.h;/home/alexander/RavenDBCppClient/include/AbstractGenericIndexCreationTask.h;/home/alexander/RavenDBCppClient/include/AbstractIndexCreationTask.h;/home/alexander/RavenDBCppClient/include/AbstractIndexCreationTaskBase.h;/home/alexander/RavenDBCppClient/include/AbstractJavaScriptIndexCreationTask.h;/home/alexander/RavenDBCppClient/include/AbstractMultiMapIndexCreationTask.h;/home/alexander/RavenDBCppClient/include/AdvancedSessionOperations.h;/home/alexander/RavenDBCppClient/include/AllTopologyNodesDownException.h;/home/alexander/RavenDBCppClient/include/AttachmentDoesNotExistException.h;/home/alexander/RavenDBCppClient/include/AuthenticationException.h;/home/alexander/RavenDBCppClient/include/AuthorizationException.h;/home/alexander/RavenDBCppClient/include/AutoSpatialMethodType.h;/home/alexander/RavenDBCppClient/include/AutoSpatialOptions.h;/home/alexander/RavenDBCppClient/include/BadRequestException.h;/home/alexander/RavenDBCppClient/include/BadResponseException.h;/home/alexander/RavenDBCppClient/include/BatchCommand.h;/home/alexander/RavenDBCppClient/include/BatchCommandResult.h;/home/alexander/RavenDBCppClient/include/BatchOperation.h;/home/alexander/RavenDBCppClient/include/BatchOptions.h;/home/alexander/RavenDBCppClient/include/BulkInsertAbortedException.h;/home/alexander/RavenDBCppClient/include/BulkInsertProtocolViolationException.h;/home/alexander/RavenDBCppClient/include/CertificateDetails.h;/home/alexander/RavenDBCppClient/include/CertificateNameMismatchException.h;/home/alexander/RavenDBCppClient/include/ChangeProcessingException.h;/home/alexander/RavenDBCppClient/include/CircleCriteria.h;/home/alexander/RavenDBCppClient/include/ClientConfiguration.h;/home/alexander/RavenDBCppClient/include/ClientVersionMismatchException.h;/home/alexander/RavenDBCppClient/include/CloseSubclauseToken.h;/home/alexander/RavenDBCppClient/include/ClusterRequestExecutor.h;/home/alexander/RavenDBCppClient/include/ClusterTopology.h;/home/alexander/RavenDBCppClient/include/ClusterTopologyResponse.h;/home/alexander/RavenDBCppClient/include/CmpXchg.h;/home/alexander/RavenDBCppClient/include/CollectionStatistics.h;/home/alexander/RavenDBCppClient/include/CommandDataBase.h;/home/alexander/RavenDBCppClient/include/CommandExecutionException.h;/home/alexander/RavenDBCppClient/include/CommandType.h;/home/alexander/RavenDBCppClient/include/CompareStringsEqualIgnoreCase.h;/home/alexander/RavenDBCppClient/include/CompareStringsLessThanIgnoreCase.h;/home/alexander/RavenDBCppClient/include/CompilationException.h;/home/alexander/RavenDBCppClient/include/ConcurrencyCheckMode.h;/home/alexander/RavenDBCppClient/include/ConcurrencyException.h;/home/alexander/RavenDBCppClient/include/ConflictException.h;/home/alexander/RavenDBCppClient/include/ConflictSolver.h;/home/alexander/RavenDBCppClient/include/Constants.h;/home/alexander/RavenDBCppClient/include/CounterIncludesToken.h;/home/alexander/RavenDBCppClient/include/CounterOverflowException.h;/home/alexander/RavenDBCppClient/include/CreateDatabaseOperation.h;/home/alexander/RavenDBCppClient/include/CurlException.h;/home/alexander/RavenDBCppClient/include/CurlHandlesHolder.h;/home/alexander/RavenDBCppClient/include/CurlResponse.h;/home/alexander/RavenDBCppClient/include/CurlSListHolder.h;/home/alexander/RavenDBCppClient/include/CurrentIndexAndNode.h;/home/alexander/RavenDBCppClient/include/DatabaseConcurrentLoadTimeoutException.h;/home/alexander/RavenDBCppClient/include/DatabaseDisabledException.h;/home/alexander/RavenDBCppClient/include/DatabaseDoesNotExistException.h;/home/alexander/RavenDBCppClient/include/DatabaseLoadFailureException.h;/home/alexander/RavenDBCppClient/include/DatabaseLoadTimeoutException.h;/home/alexander/RavenDBCppClient/include/DatabaseNotRelevantException.h;/home/alexander/RavenDBCppClient/include/DatabasePromotionStatus.h;/home/alexander/RavenDBCppClient/include/DatabasePutResult.h;/home/alexander/RavenDBCppClient/include/DatabaseRecord.h;/home/alexander/RavenDBCppClient/include/DatabaseSchemaErrorException.h;/home/alexander/RavenDBCppClient/include/DatabaseStatistics.h;/home/alexander/RavenDBCppClient/include/DatabaseTopology.h;/home/alexander/RavenDBCppClient/include/DateTimeOffset.h;/home/alexander/RavenDBCppClient/include/DeclareToken.h;/home/alexander/RavenDBCppClient/include/DeleteByQueryOperation.h;/home/alexander/RavenDBCppClient/include/DeleteCommandData.h;/home/alexander/RavenDBCppClient/include/DeleteDatabaseResult.h;/home/alexander/RavenDBCppClient/include/DeleteDatabasesOperation.h;/home/alexander/RavenDBCppClient/include/DeleteDocumentCommand.h;/home/alexander/RavenDBCppClient/include/DeleteIndexOperation.h;/home/alexander/RavenDBCppClient/include/DisableIndexOperation.h;/home/alexander/RavenDBCppClient/include/DistinctToken.h;/home/alexander/RavenDBCppClient/include/DocumentConflictException.h;/home/alexander/RavenDBCppClient/include/DocumentConventions.h;/home/alexander/RavenDBCppClient/include/DocumentDoesNotExistException.h;/home/alexander/RavenDBCppClient/include/DocumentInfo.h;/home/alexander/RavenDBCppClient/include/DocumentQuery.h;/home/alexander/RavenDBCppClient/include/DocumentQueryHelper.h;/home/alexander/RavenDBCppClient/include/DocumentSession.h;/home/alexander/RavenDBCppClient/include/DocumentSessionImpl.h;/home/alexander/RavenDBCppClient/include/DocumentStore.h;/home/alexander/RavenDBCppClient/include/DocumentStoreBase.h;/home/alexander/RavenDBCppClient/include/DocumentsById.h;/home/alexander/RavenDBCppClient/include/DocumentsByIdsMap.h;/home/alexander/RavenDBCppClient/include/DocumentsChanges.h;/home/alexander/RavenDBCppClient/include/DynamicSpatialField.h;/home/alexander/RavenDBCppClient/include/EagerSessionOperations.h;/home/alexander/RavenDBCppClient/include/EnableIndexOperation.h;/home/alexander/RavenDBCppClient/include/EntityIdHelper.h;/home/alexander/RavenDBCppClient/include/EntityIdHelperUtil.h;/home/alexander/RavenDBCppClient/include/EntityToJson.h;/home/alexander/RavenDBCppClient/include/EventArgs.h;/home/alexander/RavenDBCppClient/include/EventHandler.h;/home/alexander/RavenDBCppClient/include/EventHelper.h;/home/alexander/RavenDBCppClient/include/ExceptionDispatcher.h;/home/alexander/RavenDBCppClient/include/ExplanationOptions.h;/home/alexander/RavenDBCppClient/include/ExplanationToken.h;/home/alexander/RavenDBCppClient/include/Explanations.h;/home/alexander/RavenDBCppClient/include/FacetToken.h;/home/alexander/RavenDBCppClient/include/FieldIndexing.h;/home/alexander/RavenDBCppClient/include/FieldStorage.h;/home/alexander/RavenDBCppClient/include/FieldTermVector.h;/home/alexander/RavenDBCppClient/include/FieldsToFetchToken.h;/home/alexander/RavenDBCppClient/include/FromToken.h;/home/alexander/RavenDBCppClient/include/GenerateEntityIdOnTheClient.h;/home/alexander/RavenDBCppClient/include/GenericQueryResult.h;/home/alexander/RavenDBCppClient/include/GetClientConfigurationOperation.h;/home/alexander/RavenDBCppClient/include/GetClusterTopologyCommand.h;/home/alexander/RavenDBCppClient/include/GetCollectionStatisticsOperation.h;/home/alexander/RavenDBCppClient/include/GetCppClassName.h;/home/alexander/RavenDBCppClient/include/GetDatabaseNamesOperation.h;/home/alexander/RavenDBCppClient/include/GetDatabaseRecordOperation.h;/home/alexander/RavenDBCppClient/include/GetDatabaseTopologyCommand.h;/home/alexander/RavenDBCppClient/include/GetDocumentsCommand.h;/home/alexander/RavenDBCppClient/include/GetDocumentsResult.h;/home/alexander/RavenDBCppClient/include/GetIndexErrorsOperation.h;/home/alexander/RavenDBCppClient/include/GetIndexNamesOperation.h;/home/alexander/RavenDBCppClient/include/GetIndexOperation.h;/home/alexander/RavenDBCppClient/include/GetIndexStatisticsOperation.h;/home/alexander/RavenDBCppClient/include/GetIndexesOperation.h;/home/alexander/RavenDBCppClient/include/GetIndexesStatisticsOperation.h;/home/alexander/RavenDBCppClient/include/GetIndexingStatusOperation.h;/home/alexander/RavenDBCppClient/include/GetNextOperationIdCommand.h;/home/alexander/RavenDBCppClient/include/GetOperationStateOperation.h;/home/alexander/RavenDBCppClient/include/GetRequest.h;/home/alexander/RavenDBCppClient/include/GetResponse.h;/home/alexander/RavenDBCppClient/include/GetServerWideClientConfigurationOperation.h;/home/alexander/RavenDBCppClient/include/GetStatisticsOperation.h;/home/alexander/RavenDBCppClient/include/GetTcpInfoCommand.h;/home/alexander/RavenDBCppClient/include/GetTermsOperation.h;/home/alexander/RavenDBCppClient/include/GroupBy.h;/home/alexander/RavenDBCppClient/include/GroupByCountToken.h;/home/alexander/RavenDBCppClient/include/GroupByDocumentQuery.h;/home/alexander/RavenDBCppClient/include/GroupByField.h;/home/alexander/RavenDBCppClient/include/GroupByKeyToken.h;/home/alexander/RavenDBCppClient/include/GroupByMethod.h;/home/alexander/RavenDBCppClient/include/GroupBySumToken.h;/home/alexander/RavenDBCppClient/include/GroupByToken.h;/home/alexander/RavenDBCppClient/include/HeadDocumentCommand.h;/home/alexander/RavenDBCppClient/include/HiLoIdGenerator.h;/home/alexander/RavenDBCppClient/include/HiLoResult.h;/home/alexander/RavenDBCppClient/include/HiLoReturnCommand.h;/home/alexander/RavenDBCppClient/include/HighlightingOptions.h;/home/alexander/RavenDBCppClient/include/HighlightingToken.h;/home/alexander/RavenDBCppClient/include/Highlightings.h;/home/alexander/RavenDBCppClient/include/HttpCache.h;/home/alexander/RavenDBCppClient/include/HttpCacheItem.h;/home/alexander/RavenDBCppClient/include/HttpExtensions.h;/home/alexander/RavenDBCppClient/include/HttpStatus.h;/home/alexander/RavenDBCppClient/include/IDisposalNotification.h;/home/alexander/RavenDBCppClient/include/IDocumentQuery.h;/home/alexander/RavenDBCppClient/include/IDocumentQueryBase.h;/home/alexander/RavenDBCppClient/include/IDocumentQueryBaseSingle.h;/home/alexander/RavenDBCppClient/include/IDocumentStore.h;/home/alexander/RavenDBCppClient/include/IEnumerableQuery.h;/home/alexander/RavenDBCppClient/include/IExecutorService.h;/home/alexander/RavenDBCppClient/include/IFilterDocumentQueryBase.h;/home/alexander/RavenDBCppClient/include/IGroupByDocumentQuery.h;/home/alexander/RavenDBCppClient/include/ILazyOperation.h;/home/alexander/RavenDBCppClient/include/IMaintenanceOperation.h;/home/alexander/RavenDBCppClient/include/IMetadataDictionary.h;/home/alexander/RavenDBCppClient/include/IOperation.h;/home/alexander/RavenDBCppClient/include/IQueryBase.h;/home/alexander/RavenDBCppClient/include/IRawDocumentQuery.h;/home/alexander/RavenDBCppClient/include/IServerOperation.h;/home/alexander/RavenDBCppClient/include/IVoidMaintenanceOperation.h;/home/alexander/RavenDBCppClient/include/IVoidOperation.h;/home/alexander/RavenDBCppClient/include/IVoidServerOperation.h;/home/alexander/RavenDBCppClient/include/InMemoryDocumentSessionOperations.h;/home/alexander/RavenDBCppClient/include/IndexAlreadyExistException.h;/home/alexander/RavenDBCppClient/include/IndexBatchOptions.h;/home/alexander/RavenDBCppClient/include/IndexCompilationException.h;/home/alexander/RavenDBCppClient/include/IndexConfiguration.h;/home/alexander/RavenDBCppClient/include/IndexCreation.h;/home/alexander/RavenDBCppClient/include/IndexCreationException.h;/home/alexander/RavenDBCppClient/include/IndexDefinition.h;/home/alexander/RavenDBCppClient/include/IndexDefinitionBuilder.h;/home/alexander/RavenDBCppClient/include/IndexDeletionException.h;/home/alexander/RavenDBCppClient/include/IndexDoesNotExistException.h;/home/alexander/RavenDBCppClient/include/IndexErrors.h;/home/alexander/RavenDBCppClient/include/IndexFieldOptions.h;/home/alexander/RavenDBCppClient/include/IndexHasChangedOperation.h;/home/alexander/RavenDBCppClient/include/IndexInformation.h;/home/alexander/RavenDBCppClient/include/IndexInvalidException.h;/home/alexander/RavenDBCppClient/include/IndexLockMode.h;/home/alexander/RavenDBCppClient/include/IndexPriority.h;/home/alexander/RavenDBCppClient/include/IndexQuery.h;/home/alexander/RavenDBCppClient/include/IndexQueryBase.h;/home/alexander/RavenDBCppClient/include/IndexQueryContent.h;/home/alexander/RavenDBCppClient/include/IndexRunningStatus.h;/home/alexander/RavenDBCppClient/include/IndexState.h;/home/alexander/RavenDBCppClient/include/IndexStats.h;/home/alexander/RavenDBCppClient/include/IndexType.h;/home/alexander/RavenDBCppClient/include/IndexTypeExtensions.h;/home/alexander/RavenDBCppClient/include/IndexingError.h;/home/alexander/RavenDBCppClient/include/IndexingStatus.h;/home/alexander/RavenDBCppClient/include/Inflector.h;/home/alexander/RavenDBCppClient/include/IntersectMarkerToken.h;/home/alexander/RavenDBCppClient/include/InvalidQueryException.h;/home/alexander/RavenDBCppClient/include/JavaScriptArray.h;/home/alexander/RavenDBCppClient/include/JavaScriptException.h;/home/alexander/RavenDBCppClient/include/JavaScriptParseException.h;/home/alexander/RavenDBCppClient/include/JsonOperation.h;/home/alexander/RavenDBCppClient/include/Lazy.h;/home/alexander/RavenDBCppClient/include/LazyLoadOperation.h;/home/alexander/RavenDBCppClient/include/LazyLoaderWithInclude.h;/home/alexander/RavenDBCppClient/include/LazyMultiLoaderWithInclude.h;/home/alexander/RavenDBCppClient/include/LazyQueryOperation.h;/home/alexander/RavenDBCppClient/include/LazySessionOperations.h;/home/alexander/RavenDBCppClient/include/LazySessionOperationsImpl.h;/home/alexander/RavenDBCppClient/include/LazyStartsWithOperation.h;/home/alexander/RavenDBCppClient/include/LeaderStamp.h;/home/alexander/RavenDBCppClient/include/LicenseActivationException.h;/home/alexander/RavenDBCppClient/include/LoadOperation.h;/home/alexander/RavenDBCppClient/include/LoadStartingWithOperation.h;/home/alexander/RavenDBCppClient/include/LoadToken.h;/home/alexander/RavenDBCppClient/include/LoaderWithInclude.h;/home/alexander/RavenDBCppClient/include/MaintenanceOperationExecutor.h;/home/alexander/RavenDBCppClient/include/MetadataAsDictionary.h;/home/alexander/RavenDBCppClient/include/MethodCall.h;/home/alexander/RavenDBCppClient/include/MillisToTimespanConverter.h;/home/alexander/RavenDBCppClient/include/MoreLikeThisBase.h;/home/alexander/RavenDBCppClient/include/MoreLikeThisOptions.h;/home/alexander/RavenDBCppClient/include/MoreLikeThisScope.h;/home/alexander/RavenDBCppClient/include/MoreLikeThisToken.h;/home/alexander/RavenDBCppClient/include/MoreLikeThisUsingAnyDocument.h;/home/alexander/RavenDBCppClient/include/MoreLikeThisUsingDocument.h;/home/alexander/RavenDBCppClient/include/MoreLikeThisUsingDocumentForDocumentQuery.h;/home/alexander/RavenDBCppClient/include/MultiDatabaseHiLoIdGenerator.h;/home/alexander/RavenDBCppClient/include/MultiGetCommand.h;/home/alexander/RavenDBCppClient/include/MultiGetOperation.h;/home/alexander/RavenDBCppClient/include/MultiLoaderWithInclude.h;/home/alexander/RavenDBCppClient/include/MultiTypeHiLoIdGenerator.h;/home/alexander/RavenDBCppClient/include/NegateToken.h;/home/alexander/RavenDBCppClient/include/NextHiLoCommand.h;/home/alexander/RavenDBCppClient/include/NoLeaderException.h;/home/alexander/RavenDBCppClient/include/NodeIsPassiveException.h;/home/alexander/RavenDBCppClient/include/NodeSelector.h;/home/alexander/RavenDBCppClient/include/NodeStatus.h;/home/alexander/RavenDBCppClient/include/NonUniqueObjectException.h;/home/alexander/RavenDBCppClient/include/OpenSubclauseToken.h;/home/alexander/RavenDBCppClient/include/OperationExecutor.h;/home/alexander/RavenDBCppClient/include/OperationIdResult.h;/home/alexander/RavenDBCppClient/include/OrderByToken.h;/home/alexander/RavenDBCppClient/include/OrderingType.h;/home/alexander/RavenDBCppClient/include/Parameters.h;/home/alexander/RavenDBCppClient/include/PatchByQueryOperation.h;/home/alexander/RavenDBCppClient/include/PatchCommandData.h;/home/alexander/RavenDBCppClient/include/PatchOperation.h;/home/alexander/RavenDBCppClient/include/PatchRequest.h;/home/alexander/RavenDBCppClient/include/PatchResult.h;/home/alexander/RavenDBCppClient/include/PatchStatus.h;/home/alexander/RavenDBCppClient/include/PointField.h;/home/alexander/RavenDBCppClient/include/PutClientConfigurationOperation.h;/home/alexander/RavenDBCppClient/include/PutCommandData.h;/home/alexander/RavenDBCppClient/include/PutDocumentCommand.h;/home/alexander/RavenDBCppClient/include/PutIndexResult.h;/home/alexander/RavenDBCppClient/include/PutIndexesOperation.h;/home/alexander/RavenDBCppClient/include/PutResult.h;/home/alexander/RavenDBCppClient/include/PutServerWideClientConfigurationOperation.h;/home/alexander/RavenDBCppClient/include/Query.h;/home/alexander/RavenDBCppClient/include/QueryCommand.h;/home/alexander/RavenDBCppClient/include/QueryData.h;/home/alexander/RavenDBCppClient/include/QueryFieldUtil.h;/home/alexander/RavenDBCppClient/include/QueryHighlightings.h;/home/alexander/RavenDBCppClient/include/QueryOperation.h;/home/alexander/RavenDBCppClient/include/QueryOperationOptions.h;/home/alexander/RavenDBCppClient/include/QueryOperator.h;/home/alexander/RavenDBCppClient/include/QueryOperatorToken.h;/home/alexander/RavenDBCppClient/include/QueryResult.h;/home/alexander/RavenDBCppClient/include/QueryResultBase.h;/home/alexander/RavenDBCppClient/include/QueryStatistics.h;/home/alexander/RavenDBCppClient/include/QueryTimings.h;/home/alexander/RavenDBCppClient/include/QueryToken.h;/home/alexander/RavenDBCppClient/include/RavenCommand.h;/home/alexander/RavenDBCppClient/include/RavenException.h;/home/alexander/RavenDBCppClient/include/RawDocumentQuery.h;/home/alexander/RavenDBCppClient/include/ReadBalanceBehavior.h;/home/alexander/RavenDBCppClient/include/ReplicationBatchOptions.h;/home/alexander/RavenDBCppClient/include/RequestExecutor.h;/home/alexander/RavenDBCppClient/include/ResetIndexOperation.h;/home/alexander/RavenDBCppClient/include/ResponseDisposeHandling.h;/home/alexander/RavenDBCppClient/include/ResponseTimeInformation.h;/home/alexander/RavenDBCppClient/include/RevisionsDisabledException.h;/home/alexander/RavenDBCppClient/include/RouteNotFoundException.h;/home/alexander/RavenDBCppClient/include/ScriptResolver.h;/home/alexander/RavenDBCppClient/include/SearchOperator.h;/home/alexander/RavenDBCppClient/include/SecurityException.h;/home/alexander/RavenDBCppClient/include/ServerLoadFailureException.h;/home/alexander/RavenDBCppClient/include/ServerNode.h;/home/alexander/RavenDBCppClient/include/ServerOperationExecutor.h;/home/alexander/RavenDBCppClient/include/SessionCreatedEventArgs.h;/home/alexander/RavenDBCppClient/include/SessionInfo.h;/home/alexander/RavenDBCppClient/include/SessionOperationExecutor.h;/home/alexander/RavenDBCppClient/include/SessionOptions.h;/home/alexander/RavenDBCppClient/include/SetIndexesLockOperation.h;/home/alexander/RavenDBCppClient/include/SetIndexesPriorityOperation.h;/home/alexander/RavenDBCppClient/include/ShapeToken.h;/home/alexander/RavenDBCppClient/include/SimpleStopWatch.h;/home/alexander/RavenDBCppClient/include/Size.h;/home/alexander/RavenDBCppClient/include/SpatialCriteria.h;/home/alexander/RavenDBCppClient/include/SpatialCriteriaFactory.h;/home/alexander/RavenDBCppClient/include/SpatialFieldType.h;/home/alexander/RavenDBCppClient/include/SpatialOptions.h;/home/alexander/RavenDBCppClient/include/SpatialOptionsFactory.h;/home/alexander/RavenDBCppClient/include/SpatialRelation.h;/home/alexander/RavenDBCppClient/include/SpatialSearchStrategy.h;/home/alexander/RavenDBCppClient/include/SpatialUnits.h;/home/alexander/RavenDBCppClient/include/StartIndexOperation.h;/home/alexander/RavenDBCppClient/include/StartIndexingOperationh.h;/home/alexander/RavenDBCppClient/include/StopIndexOperation.h;/home/alexander/RavenDBCppClient/include/StopIndexingOperation.h;/home/alexander/RavenDBCppClient/include/StringDistanceTypes.h;/home/alexander/RavenDBCppClient/include/SubscriberErrorException.h;/home/alexander/RavenDBCppClient/include/SubscriptionChangeVectorUpdateConcurrencyException.h;/home/alexander/RavenDBCppClient/include/SubscriptionClosedException.h;/home/alexander/RavenDBCppClient/include/SubscriptionDoesNotBelongToNodeException.h;/home/alexander/RavenDBCppClient/include/SubscriptionDoesNotExistException.h;/home/alexander/RavenDBCppClient/include/SubscriptionException.h;/home/alexander/RavenDBCppClient/include/SubscriptionInUseException.h;/home/alexander/RavenDBCppClient/include/SubscriptionInvalidStateException.h;/home/alexander/RavenDBCppClient/include/SuggestToken.h;/home/alexander/RavenDBCppClient/include/SuggestionBase.h;/home/alexander/RavenDBCppClient/include/SuggestionOptions.h;/home/alexander/RavenDBCppClient/include/SuggestionSortMode.h;/home/alexander/RavenDBCppClient/include/SuggestionWithTerm.h;/home/alexander/RavenDBCppClient/include/SuggestionWithTerms.h;/home/alexander/RavenDBCppClient/include/Supplier.h;/home/alexander/RavenDBCppClient/include/TasksExecutor.h;/home/alexander/RavenDBCppClient/include/TasksScheduler.h;/home/alexander/RavenDBCppClient/include/TcpConnectionInfo.h;/home/alexander/RavenDBCppClient/include/TermsQueryResult.h;/home/alexander/RavenDBCppClient/include/TimeoutException.h;/home/alexander/RavenDBCppClient/include/Timer.h;/home/alexander/RavenDBCppClient/include/TimingsToken.h;/home/alexander/RavenDBCppClient/include/Topology.h;/home/alexander/RavenDBCppClient/include/TransactionMode.h;/home/alexander/RavenDBCppClient/include/TrueToken.h;/home/alexander/RavenDBCppClient/include/UnsuccessfulRequestException.h;/home/alexander/RavenDBCppClient/include/VoidRavenCommand.h;/home/alexander/RavenDBCppClient/include/WhereOperator.h;/home/alexander/RavenDBCppClient/include/WhereParams.h;/home/alexander/RavenDBCppClient/include/WhereToken.h;/home/alexander/RavenDBCppClient/include/WktCriteria.h;/home/alexander/RavenDBCppClient/include/WktField.h;/home/alexander/RavenDBCppClient/include/json_utils.h;/home/alexander/RavenDBCppClient/include/raven_cpp_client.h;/home/alexander/RavenDBCppClient/include/ravendb.h;/home/alexander/RavenDBCppClient/include/stdafx.h;/home/alexander/RavenDBCppClient/include/targetver.h;/home/alexander/RavenDBCppClient/include/utils.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/alexander/RavenDBCppClient/include" TYPE FILE FILES
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/AbstractDocumentQuery.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/AbstractGenericIndexCreationTask.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/AbstractIndexCreationTask.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/AbstractIndexCreationTaskBase.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/AbstractJavaScriptIndexCreationTask.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/AbstractMultiMapIndexCreationTask.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/AdvancedSessionOperations.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/AllTopologyNodesDownException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/AttachmentDoesNotExistException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/AuthenticationException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/AuthorizationException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/AutoSpatialMethodType.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/AutoSpatialOptions.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/BadRequestException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/BadResponseException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/BatchCommand.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/BatchCommandResult.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/BatchOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/BatchOptions.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/BulkInsertAbortedException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/BulkInsertProtocolViolationException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/CertificateDetails.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/CertificateNameMismatchException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/ChangeProcessingException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/CircleCriteria.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/ClientConfiguration.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/ClientVersionMismatchException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/CloseSubclauseToken.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/ClusterRequestExecutor.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/ClusterTopology.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/ClusterTopologyResponse.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/CmpXchg.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/CollectionStatistics.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/CommandDataBase.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/CommandExecutionException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/CommandType.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/CompareStringsEqualIgnoreCase.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/CompareStringsLessThanIgnoreCase.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/CompilationException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/ConcurrencyCheckMode.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/ConcurrencyException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/ConflictException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/ConflictSolver.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/Constants.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/CounterIncludesToken.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/CounterOverflowException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/CreateDatabaseOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/CurlException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/CurlHandlesHolder.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/CurlResponse.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/CurlSListHolder.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/CurrentIndexAndNode.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/DatabaseConcurrentLoadTimeoutException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/DatabaseDisabledException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/DatabaseDoesNotExistException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/DatabaseLoadFailureException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/DatabaseLoadTimeoutException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/DatabaseNotRelevantException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/DatabasePromotionStatus.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/DatabasePutResult.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/DatabaseRecord.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/DatabaseSchemaErrorException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/DatabaseStatistics.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/DatabaseTopology.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/DateTimeOffset.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/DeclareToken.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/DeleteByQueryOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/DeleteCommandData.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/DeleteDatabaseResult.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/DeleteDatabasesOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/DeleteDocumentCommand.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/DeleteIndexOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/DisableIndexOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/DistinctToken.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/DocumentConflictException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/DocumentConventions.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/DocumentDoesNotExistException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/DocumentInfo.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/DocumentQuery.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/DocumentQueryHelper.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/DocumentSession.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/DocumentSessionImpl.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/DocumentStore.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/DocumentStoreBase.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/DocumentsById.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/DocumentsByIdsMap.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/DocumentsChanges.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/DynamicSpatialField.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/EagerSessionOperations.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/EnableIndexOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/EntityIdHelper.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/EntityIdHelperUtil.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/EntityToJson.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/EventArgs.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/EventHandler.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/EventHelper.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/ExceptionDispatcher.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/ExplanationOptions.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/ExplanationToken.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/Explanations.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/FacetToken.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/FieldIndexing.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/FieldStorage.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/FieldTermVector.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/FieldsToFetchToken.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/FromToken.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/GenerateEntityIdOnTheClient.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/GenericQueryResult.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/GetClientConfigurationOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/GetClusterTopologyCommand.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/GetCollectionStatisticsOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/GetCppClassName.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/GetDatabaseNamesOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/GetDatabaseRecordOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/GetDatabaseTopologyCommand.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/GetDocumentsCommand.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/GetDocumentsResult.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/GetIndexErrorsOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/GetIndexNamesOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/GetIndexOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/GetIndexStatisticsOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/GetIndexesOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/GetIndexesStatisticsOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/GetIndexingStatusOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/GetNextOperationIdCommand.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/GetOperationStateOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/GetRequest.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/GetResponse.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/GetServerWideClientConfigurationOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/GetStatisticsOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/GetTcpInfoCommand.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/GetTermsOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/GroupBy.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/GroupByCountToken.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/GroupByDocumentQuery.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/GroupByField.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/GroupByKeyToken.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/GroupByMethod.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/GroupBySumToken.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/GroupByToken.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/HeadDocumentCommand.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/HiLoIdGenerator.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/HiLoResult.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/HiLoReturnCommand.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/HighlightingOptions.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/HighlightingToken.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/Highlightings.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/HttpCache.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/HttpCacheItem.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/HttpExtensions.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/HttpStatus.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IDisposalNotification.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IDocumentQuery.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IDocumentQueryBase.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IDocumentQueryBaseSingle.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IDocumentStore.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IEnumerableQuery.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IExecutorService.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IFilterDocumentQueryBase.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IGroupByDocumentQuery.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/ILazyOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IMaintenanceOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IMetadataDictionary.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IQueryBase.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IRawDocumentQuery.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IServerOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IVoidMaintenanceOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IVoidOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IVoidServerOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/InMemoryDocumentSessionOperations.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IndexAlreadyExistException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IndexBatchOptions.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IndexCompilationException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IndexConfiguration.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IndexCreation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IndexCreationException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IndexDefinition.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IndexDefinitionBuilder.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IndexDeletionException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IndexDoesNotExistException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IndexErrors.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IndexFieldOptions.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IndexHasChangedOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IndexInformation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IndexInvalidException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IndexLockMode.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IndexPriority.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IndexQuery.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IndexQueryBase.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IndexQueryContent.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IndexRunningStatus.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IndexState.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IndexStats.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IndexType.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IndexTypeExtensions.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IndexingError.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IndexingStatus.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/Inflector.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/IntersectMarkerToken.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/InvalidQueryException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/JavaScriptArray.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/JavaScriptException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/JavaScriptParseException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/JsonOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/Lazy.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/LazyLoadOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/LazyLoaderWithInclude.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/LazyMultiLoaderWithInclude.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/LazyQueryOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/LazySessionOperations.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/LazySessionOperationsImpl.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/LazyStartsWithOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/LeaderStamp.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/LicenseActivationException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/LoadOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/LoadStartingWithOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/LoadToken.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/LoaderWithInclude.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/MaintenanceOperationExecutor.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/MetadataAsDictionary.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/MethodCall.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/MillisToTimespanConverter.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/MoreLikeThisBase.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/MoreLikeThisOptions.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/MoreLikeThisScope.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/MoreLikeThisToken.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/MoreLikeThisUsingAnyDocument.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/MoreLikeThisUsingDocument.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/MoreLikeThisUsingDocumentForDocumentQuery.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/MultiDatabaseHiLoIdGenerator.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/MultiGetCommand.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/MultiGetOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/MultiLoaderWithInclude.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/MultiTypeHiLoIdGenerator.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/NegateToken.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/NextHiLoCommand.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/NoLeaderException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/NodeIsPassiveException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/NodeSelector.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/NodeStatus.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/NonUniqueObjectException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/OpenSubclauseToken.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/OperationExecutor.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/OperationIdResult.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/OrderByToken.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/OrderingType.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/Parameters.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/PatchByQueryOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/PatchCommandData.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/PatchOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/PatchRequest.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/PatchResult.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/PatchStatus.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/PointField.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/PutClientConfigurationOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/PutCommandData.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/PutDocumentCommand.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/PutIndexResult.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/PutIndexesOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/PutResult.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/PutServerWideClientConfigurationOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/Query.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/QueryCommand.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/QueryData.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/QueryFieldUtil.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/QueryHighlightings.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/QueryOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/QueryOperationOptions.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/QueryOperator.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/QueryOperatorToken.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/QueryResult.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/QueryResultBase.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/QueryStatistics.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/QueryTimings.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/QueryToken.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/RavenCommand.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/RavenException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/RawDocumentQuery.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/ReadBalanceBehavior.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/ReplicationBatchOptions.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/RequestExecutor.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/ResetIndexOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/ResponseDisposeHandling.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/ResponseTimeInformation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/RevisionsDisabledException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/RouteNotFoundException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/ScriptResolver.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/SearchOperator.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/SecurityException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/ServerLoadFailureException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/ServerNode.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/ServerOperationExecutor.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/SessionCreatedEventArgs.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/SessionInfo.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/SessionOperationExecutor.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/SessionOptions.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/SetIndexesLockOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/SetIndexesPriorityOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/ShapeToken.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/SimpleStopWatch.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/Size.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/SpatialCriteria.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/SpatialCriteriaFactory.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/SpatialFieldType.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/SpatialOptions.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/SpatialOptionsFactory.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/SpatialRelation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/SpatialSearchStrategy.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/SpatialUnits.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/StartIndexOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/StartIndexingOperationh.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/StopIndexOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/StopIndexingOperation.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/StringDistanceTypes.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/SubscriberErrorException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/SubscriptionChangeVectorUpdateConcurrencyException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/SubscriptionClosedException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/SubscriptionDoesNotBelongToNodeException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/SubscriptionDoesNotExistException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/SubscriptionException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/SubscriptionInUseException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/SubscriptionInvalidStateException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/SuggestToken.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/SuggestionBase.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/SuggestionOptions.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/SuggestionSortMode.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/SuggestionWithTerm.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/SuggestionWithTerms.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/Supplier.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/TasksExecutor.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/TasksScheduler.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/TcpConnectionInfo.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/TermsQueryResult.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/TimeoutException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/Timer.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/TimingsToken.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/Topology.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/TransactionMode.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/TrueToken.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/UnsuccessfulRequestException.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/VoidRavenCommand.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/WhereOperator.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/WhereParams.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/WhereToken.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/WktCriteria.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/WktField.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/json_utils.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/raven_cpp_client.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/ravendb.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/stdafx.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/targetver.h"
    "/home/alexander/RavenDB_Client/raven-cpp-client/Raven.CppClient/utils.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/alexander/RavenDBCppClient/external/json.hpp;/home/alexander/RavenDBCppClient/external/xxhash.hpp")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/alexander/RavenDBCppClient/external" TYPE FILE FILES
    "/home/alexander/RavenDB_Client/raven-cpp-client/external/json.hpp"
    "/home/alexander/RavenDB_Client/raven-cpp-client/external/xxhash.hpp"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/alexander/RavenDBCppClient/ca_bundle/ca-bundle.crt;/home/alexander/RavenDBCppClient/ca_bundle/ca-bundle.trust.crt")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/alexander/RavenDBCppClient/ca_bundle" TYPE FILE FILES
    "/home/alexander/RavenDB_Client/raven-cpp-client/ca_bundle/ca-bundle.crt"
    "/home/alexander/RavenDB_Client/raven-cpp-client/ca_bundle/ca-bundle.trust.crt"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/alexander/RavenDB_Client/raven-cpp-client/cmake-build-debug/Raven.CppClient/cmake_install.cmake")
  include("/home/alexander/RavenDB_Client/raven-cpp-client/cmake-build-debug/Tryouts/cmake_install.cmake")
  include("/home/alexander/RavenDB_Client/raven-cpp-client/cmake-build-debug/Raven.CppClient.Tests/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/alexander/RavenDB_Client/raven-cpp-client/cmake-build-debug/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
