# Install script for directory: /home/alexander/RavenDB_Client/raven-cpp-client

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
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
  if(EXISTS "$ENV{DESTDIR}/usr/local/lib/libRaven_CppClient.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/usr/local/lib/libRaven_CppClient.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/usr/local/lib/libRaven_CppClient.so"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/lib/libRaven_CppClient.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/lib" TYPE SHARED_LIBRARY FILES "/home/alexander/RavenDB_Client/raven-cpp-client/cmake-build-debug/Raven.CppClient/libRaven_CppClient.so")
  if(EXISTS "$ENV{DESTDIR}/usr/local/lib/libRaven_CppClient.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/usr/local/lib/libRaven_CppClient.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/usr/local/lib/libRaven_CppClient.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/include/AbstractDocumentQuery.h;/usr/local/include/AbstractGenericIndexCreationTask.h;/usr/local/include/AbstractIndexCreationTask.h;/usr/local/include/AbstractIndexCreationTaskBase.h;/usr/local/include/AbstractJavaScriptIndexCreationTask.h;/usr/local/include/AbstractMultiMapIndexCreationTask.h;/usr/local/include/AdvancedSessionOperations.h;/usr/local/include/AllTopologyNodesDownException.h;/usr/local/include/AttachmentDoesNotExistException.h;/usr/local/include/AuthenticationException.h;/usr/local/include/AuthorizationException.h;/usr/local/include/AutoSpatialMethodType.h;/usr/local/include/AutoSpatialOptions.h;/usr/local/include/BadRequestException.h;/usr/local/include/BadResponseException.h;/usr/local/include/BatchCommand.h;/usr/local/include/BatchCommandResult.h;/usr/local/include/BatchOperation.h;/usr/local/include/BatchOptions.h;/usr/local/include/BulkInsertAbortedException.h;/usr/local/include/BulkInsertProtocolViolationException.h;/usr/local/include/CertificateDetails.h;/usr/local/include/CertificateNameMismatchException.h;/usr/local/include/ChangeProcessingException.h;/usr/local/include/CircleCriteria.h;/usr/local/include/ClientConfiguration.h;/usr/local/include/ClientVersionMismatchException.h;/usr/local/include/CloseSubclauseToken.h;/usr/local/include/ClusterRequestExecutor.h;/usr/local/include/ClusterTopology.h;/usr/local/include/ClusterTopologyResponse.h;/usr/local/include/CmpXchg.h;/usr/local/include/CollectionStatistics.h;/usr/local/include/CommandDataBase.h;/usr/local/include/CommandExecutionException.h;/usr/local/include/CommandType.h;/usr/local/include/CompareStringsEqualIgnoreCase.h;/usr/local/include/CompareStringsLessThanIgnoreCase.h;/usr/local/include/CompilationException.h;/usr/local/include/ConcurrencyCheckMode.h;/usr/local/include/ConcurrencyException.h;/usr/local/include/ConflictException.h;/usr/local/include/ConflictSolver.h;/usr/local/include/Constants.h;/usr/local/include/CounterIncludesToken.h;/usr/local/include/CounterOverflowException.h;/usr/local/include/CreateDatabaseOperation.h;/usr/local/include/CurlException.h;/usr/local/include/CurlHandlesHolder.h;/usr/local/include/CurlResponse.h;/usr/local/include/CurlSListHolder.h;/usr/local/include/CurrentIndexAndNode.h;/usr/local/include/DatabaseConcurrentLoadTimeoutException.h;/usr/local/include/DatabaseDisabledException.h;/usr/local/include/DatabaseDoesNotExistException.h;/usr/local/include/DatabaseLoadFailureException.h;/usr/local/include/DatabaseLoadTimeoutException.h;/usr/local/include/DatabaseNotRelevantException.h;/usr/local/include/DatabasePromotionStatus.h;/usr/local/include/DatabasePutResult.h;/usr/local/include/DatabaseRecord.h;/usr/local/include/DatabaseSchemaErrorException.h;/usr/local/include/DatabaseStatistics.h;/usr/local/include/DatabaseTopology.h;/usr/local/include/DateTimeOffset.h;/usr/local/include/DeclareToken.h;/usr/local/include/DeleteByQueryOperation.h;/usr/local/include/DeleteCommandData.h;/usr/local/include/DeleteDatabaseResult.h;/usr/local/include/DeleteDatabasesOperation.h;/usr/local/include/DeleteDocumentCommand.h;/usr/local/include/DeleteIndexOperation.h;/usr/local/include/DisableIndexOperation.h;/usr/local/include/DistinctToken.h;/usr/local/include/DocumentConflictException.h;/usr/local/include/DocumentConventions.h;/usr/local/include/DocumentDoesNotExistException.h;/usr/local/include/DocumentInfo.h;/usr/local/include/DocumentQuery.h;/usr/local/include/DocumentQueryHelper.h;/usr/local/include/DocumentSession.h;/usr/local/include/DocumentSessionImpl.h;/usr/local/include/DocumentStore.h;/usr/local/include/DocumentStoreBase.h;/usr/local/include/DocumentsById.h;/usr/local/include/DocumentsByIdsMap.h;/usr/local/include/DocumentsChanges.h;/usr/local/include/DynamicSpatialField.h;/usr/local/include/EagerSessionOperations.h;/usr/local/include/EnableIndexOperation.h;/usr/local/include/EntityIdHelper.h;/usr/local/include/EntityIdHelperUtil.h;/usr/local/include/EntityToJson.h;/usr/local/include/EventArgs.h;/usr/local/include/EventHandler.h;/usr/local/include/EventHelper.h;/usr/local/include/ExceptionDispatcher.h;/usr/local/include/ExplanationOptions.h;/usr/local/include/ExplanationToken.h;/usr/local/include/Explanations.h;/usr/local/include/FacetToken.h;/usr/local/include/FieldIndexing.h;/usr/local/include/FieldStorage.h;/usr/local/include/FieldTermVector.h;/usr/local/include/FieldsToFetchToken.h;/usr/local/include/FromToken.h;/usr/local/include/GenerateEntityIdOnTheClient.h;/usr/local/include/GenericQueryResult.h;/usr/local/include/GetClientConfigurationOperation.h;/usr/local/include/GetClusterTopologyCommand.h;/usr/local/include/GetCollectionStatisticsOperation.h;/usr/local/include/GetCppClassName.h;/usr/local/include/GetDatabaseNamesOperation.h;/usr/local/include/GetDatabaseRecordOperation.h;/usr/local/include/GetDatabaseTopologyCommand.h;/usr/local/include/GetDocumentsCommand.h;/usr/local/include/GetDocumentsResult.h;/usr/local/include/GetIndexErrorsOperation.h;/usr/local/include/GetIndexNamesOperation.h;/usr/local/include/GetIndexOperation.h;/usr/local/include/GetIndexStatisticsOperation.h;/usr/local/include/GetIndexesOperation.h;/usr/local/include/GetIndexesStatisticsOperation.h;/usr/local/include/GetIndexingStatusOperation.h;/usr/local/include/GetNextOperationIdCommand.h;/usr/local/include/GetOperationStateOperation.h;/usr/local/include/GetRequest.h;/usr/local/include/GetResponse.h;/usr/local/include/GetServerWideClientConfigurationOperation.h;/usr/local/include/GetStatisticsOperation.h;/usr/local/include/GetTcpInfoCommand.h;/usr/local/include/GetTermsOperation.h;/usr/local/include/GroupBy.h;/usr/local/include/GroupByCountToken.h;/usr/local/include/GroupByDocumentQuery.h;/usr/local/include/GroupByField.h;/usr/local/include/GroupByKeyToken.h;/usr/local/include/GroupByMethod.h;/usr/local/include/GroupBySumToken.h;/usr/local/include/GroupByToken.h;/usr/local/include/HeadDocumentCommand.h;/usr/local/include/HiLoIdGenerator.h;/usr/local/include/HiLoResult.h;/usr/local/include/HiLoReturnCommand.h;/usr/local/include/HighlightingOptions.h;/usr/local/include/HighlightingToken.h;/usr/local/include/Highlightings.h;/usr/local/include/HttpCache.h;/usr/local/include/HttpCacheItem.h;/usr/local/include/HttpExtensions.h;/usr/local/include/HttpStatus.h;/usr/local/include/IDisposalNotification.h;/usr/local/include/IDocumentQuery.h;/usr/local/include/IDocumentQueryBase.h;/usr/local/include/IDocumentQueryBaseSingle.h;/usr/local/include/IDocumentStore.h;/usr/local/include/IEnumerableQuery.h;/usr/local/include/IExecutorService.h;/usr/local/include/IFilterDocumentQueryBase.h;/usr/local/include/IGroupByDocumentQuery.h;/usr/local/include/ILazyOperation.h;/usr/local/include/IMaintenanceOperation.h;/usr/local/include/IMetadataDictionary.h;/usr/local/include/IOperation.h;/usr/local/include/IQueryBase.h;/usr/local/include/IRawDocumentQuery.h;/usr/local/include/IServerOperation.h;/usr/local/include/IVoidMaintenanceOperation.h;/usr/local/include/IVoidOperation.h;/usr/local/include/IVoidServerOperation.h;/usr/local/include/InMemoryDocumentSessionOperations.h;/usr/local/include/IndexAlreadyExistException.h;/usr/local/include/IndexBatchOptions.h;/usr/local/include/IndexCompilationException.h;/usr/local/include/IndexConfiguration.h;/usr/local/include/IndexCreation.h;/usr/local/include/IndexCreationException.h;/usr/local/include/IndexDefinition.h;/usr/local/include/IndexDefinitionBuilder.h;/usr/local/include/IndexDeletionException.h;/usr/local/include/IndexDoesNotExistException.h;/usr/local/include/IndexErrors.h;/usr/local/include/IndexFieldOptions.h;/usr/local/include/IndexHasChangedOperation.h;/usr/local/include/IndexInformation.h;/usr/local/include/IndexInvalidException.h;/usr/local/include/IndexLockMode.h;/usr/local/include/IndexPriority.h;/usr/local/include/IndexQuery.h;/usr/local/include/IndexQueryBase.h;/usr/local/include/IndexQueryContent.h;/usr/local/include/IndexRunningStatus.h;/usr/local/include/IndexState.h;/usr/local/include/IndexStats.h;/usr/local/include/IndexType.h;/usr/local/include/IndexTypeExtensions.h;/usr/local/include/IndexingError.h;/usr/local/include/IndexingStatus.h;/usr/local/include/Inflector.h;/usr/local/include/IntersectMarkerToken.h;/usr/local/include/InvalidQueryException.h;/usr/local/include/JavaScriptArray.h;/usr/local/include/JavaScriptException.h;/usr/local/include/JavaScriptParseException.h;/usr/local/include/JsonOperation.h;/usr/local/include/Lazy.h;/usr/local/include/LazyLoadOperation.h;/usr/local/include/LazyLoaderWithInclude.h;/usr/local/include/LazyMultiLoaderWithInclude.h;/usr/local/include/LazyQueryOperation.h;/usr/local/include/LazySessionOperations.h;/usr/local/include/LazySessionOperationsImpl.h;/usr/local/include/LazyStartsWithOperation.h;/usr/local/include/LeaderStamp.h;/usr/local/include/LicenseActivationException.h;/usr/local/include/LoadOperation.h;/usr/local/include/LoadStartingWithOperation.h;/usr/local/include/LoadToken.h;/usr/local/include/LoaderWithInclude.h;/usr/local/include/MaintenanceOperationExecutor.h;/usr/local/include/MetadataAsDictionary.h;/usr/local/include/MethodCall.h;/usr/local/include/MillisToTimespanConverter.h;/usr/local/include/MoreLikeThisBase.h;/usr/local/include/MoreLikeThisOptions.h;/usr/local/include/MoreLikeThisScope.h;/usr/local/include/MoreLikeThisToken.h;/usr/local/include/MoreLikeThisUsingAnyDocument.h;/usr/local/include/MoreLikeThisUsingDocument.h;/usr/local/include/MoreLikeThisUsingDocumentForDocumentQuery.h;/usr/local/include/MultiDatabaseHiLoIdGenerator.h;/usr/local/include/MultiGetCommand.h;/usr/local/include/MultiGetOperation.h;/usr/local/include/MultiLoaderWithInclude.h;/usr/local/include/MultiTypeHiLoIdGenerator.h;/usr/local/include/NegateToken.h;/usr/local/include/NextHiLoCommand.h;/usr/local/include/NoLeaderException.h;/usr/local/include/NodeIsPassiveException.h;/usr/local/include/NodeSelector.h;/usr/local/include/NodeStatus.h;/usr/local/include/NonUniqueObjectException.h;/usr/local/include/OpenSubclauseToken.h;/usr/local/include/OperationExecutor.h;/usr/local/include/OperationIdResult.h;/usr/local/include/OrderByToken.h;/usr/local/include/OrderingType.h;/usr/local/include/Parameters.h;/usr/local/include/PatchByQueryOperation.h;/usr/local/include/PatchCommandData.h;/usr/local/include/PatchOperation.h;/usr/local/include/PatchRequest.h;/usr/local/include/PatchResult.h;/usr/local/include/PatchStatus.h;/usr/local/include/PointField.h;/usr/local/include/PutClientConfigurationOperation.h;/usr/local/include/PutCommandData.h;/usr/local/include/PutDocumentCommand.h;/usr/local/include/PutIndexResult.h;/usr/local/include/PutIndexesOperation.h;/usr/local/include/PutResult.h;/usr/local/include/PutServerWideClientConfigurationOperation.h;/usr/local/include/Query.h;/usr/local/include/QueryCommand.h;/usr/local/include/QueryData.h;/usr/local/include/QueryFieldUtil.h;/usr/local/include/QueryHighlightings.h;/usr/local/include/QueryOperation.h;/usr/local/include/QueryOperationOptions.h;/usr/local/include/QueryOperator.h;/usr/local/include/QueryOperatorToken.h;/usr/local/include/QueryResult.h;/usr/local/include/QueryResultBase.h;/usr/local/include/QueryStatistics.h;/usr/local/include/QueryTimings.h;/usr/local/include/QueryToken.h;/usr/local/include/RavenCommand.h;/usr/local/include/RavenException.h;/usr/local/include/RawDocumentQuery.h;/usr/local/include/ReadBalanceBehavior.h;/usr/local/include/ReplicationBatchOptions.h;/usr/local/include/RequestExecutor.h;/usr/local/include/ResetIndexOperation.h;/usr/local/include/ResponseDisposeHandling.h;/usr/local/include/ResponseTimeInformation.h;/usr/local/include/RevisionsDisabledException.h;/usr/local/include/RouteNotFoundException.h;/usr/local/include/ScriptResolver.h;/usr/local/include/SearchOperator.h;/usr/local/include/SecurityException.h;/usr/local/include/ServerLoadFailureException.h;/usr/local/include/ServerNode.h;/usr/local/include/ServerOperationExecutor.h;/usr/local/include/SessionCreatedEventArgs.h;/usr/local/include/SessionInfo.h;/usr/local/include/SessionOperationExecutor.h;/usr/local/include/SessionOptions.h;/usr/local/include/SetIndexesLockOperation.h;/usr/local/include/SetIndexesPriorityOperation.h;/usr/local/include/ShapeToken.h;/usr/local/include/SimpleStopWatch.h;/usr/local/include/Size.h;/usr/local/include/SpatialCriteria.h;/usr/local/include/SpatialCriteriaFactory.h;/usr/local/include/SpatialFieldType.h;/usr/local/include/SpatialOptions.h;/usr/local/include/SpatialOptionsFactory.h;/usr/local/include/SpatialRelation.h;/usr/local/include/SpatialSearchStrategy.h;/usr/local/include/SpatialUnits.h;/usr/local/include/StartIndexOperation.h;/usr/local/include/StartIndexingOperationh.h;/usr/local/include/StopIndexOperation.h;/usr/local/include/StopIndexingOperation.h;/usr/local/include/StringDistanceTypes.h;/usr/local/include/SubscriberErrorException.h;/usr/local/include/SubscriptionChangeVectorUpdateConcurrencyException.h;/usr/local/include/SubscriptionClosedException.h;/usr/local/include/SubscriptionDoesNotBelongToNodeException.h;/usr/local/include/SubscriptionDoesNotExistException.h;/usr/local/include/SubscriptionException.h;/usr/local/include/SubscriptionInUseException.h;/usr/local/include/SubscriptionInvalidStateException.h;/usr/local/include/SuggestToken.h;/usr/local/include/SuggestionBase.h;/usr/local/include/SuggestionOptions.h;/usr/local/include/SuggestionSortMode.h;/usr/local/include/SuggestionWithTerm.h;/usr/local/include/SuggestionWithTerms.h;/usr/local/include/Supplier.h;/usr/local/include/TasksExecutor.h;/usr/local/include/TasksScheduler.h;/usr/local/include/TcpConnectionInfo.h;/usr/local/include/TermsQueryResult.h;/usr/local/include/TimeoutException.h;/usr/local/include/Timer.h;/usr/local/include/TimingsToken.h;/usr/local/include/Topology.h;/usr/local/include/TransactionMode.h;/usr/local/include/TrueToken.h;/usr/local/include/UnsuccessfulRequestException.h;/usr/local/include/VoidRavenCommand.h;/usr/local/include/WhereOperator.h;/usr/local/include/WhereParams.h;/usr/local/include/WhereToken.h;/usr/local/include/WktCriteria.h;/usr/local/include/WktField.h;/usr/local/include/json_utils.h;/usr/local/include/raven_cpp_client.h;/usr/local/include/ravendb.h;/usr/local/include/stdafx.h;/usr/local/include/targetver.h;/usr/local/include/utils.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/include" TYPE FILE FILES
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
   "/usr/local/external/json.hpp;/usr/local/external/xxhash.hpp")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/external" TYPE FILE FILES
    "/home/alexander/RavenDB_Client/raven-cpp-client/external/json.hpp"
    "/home/alexander/RavenDB_Client/raven-cpp-client/external/xxhash.hpp"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/ca_bundle/ca-bundle.crt;/usr/local/ca_bundle/ca-bundle.trust.crt")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/ca_bundle" TYPE FILE FILES
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
