#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "User.h"
#include "EntityIdHelperUtil.h"
#include "AbstractIndexCreationTask.h"
#include "MaintenanceOperationExecutor.h"
#include "GetIndexNamesOperation.h"
#include "DeleteIndexOperation.h"
#include "DisableIndexOperation.h"
#include "GetIndexingStatusOperation.h"
#include "EnableIndexOperation.h"
#include "GetIndexesOperation.h"
#include "GetIndexesStatisticsOperation.h"
#include "DocumentSession.h"
#include "GetTermsOperation.h"
#include "PutIndexesOperation.h"
#include "IndexHasChangedOperation.h"
#include "StopIndexingOperation.h"
#include "StartIndexingOperationh.h"
#include "StopIndexOperation.h"
#include "StartIndexOperation.h"
#include "SetIndexesLockOperation.h"
#include "GetIndexOperation.h"
#include "SetIndexesPriorityOperation.h"
#include "GetIndexErrorsOperation.h"
#include "GetIndexStatisticsOperation.h"
#include "GetStatisticsOperation.h"

namespace ravendb::client::tests::client::documents::operations::indexes
{
	class IndexOperationsTest : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<ravendb::client::documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}

		static void SetUpTestCase()
		{
			REGISTER_ID_PROPERTY_FOR(ravendb::client::tests::infrastructure::entities::User, id);
		}

		class Users_Index : public ravendb::client::documents::indexes::AbstractIndexCreationTask
		{
		public:
			~Users_Index() override = default;
			Users_Index()
			{
				SET_DEFAULT_INDEX_NAME();
				map = "from u in docs.Users select new { u.name }";
			}
		};

		class UsersInvalidIndex : public ravendb::client::documents::indexes::AbstractIndexCreationTask
		{
		public:
			~UsersInvalidIndex() override = default;
			UsersInvalidIndex()
			{
				SET_DEFAULT_INDEX_NAME();
				map = "from u in docs.Users select new { a = 5 / u.age }";
			}
		};
	};

	TEST_F(IndexOperationsTest, CanDeleteIndex)
	{
		auto store = get_document_store(TEST_NAME);

		Users_Index().execute(store);

		auto index_names = store->maintenance()->send(
			ravendb::client::documents::operations::indexes::GetIndexNamesOperation(0, 10));
		ASSERT_NE(std::find(index_names->cbegin(), index_names->cend(), "Users/Index"),
			index_names->cend());

		store->maintenance()->send(
			ravendb::client::documents::operations::indexes::DeleteIndexOperation(Users_Index().get_index_name()));

		index_names = store->maintenance()->send(
			ravendb::client::documents::operations::indexes::GetIndexNamesOperation(0, 10));

		ASSERT_TRUE(index_names->empty());
	}

	TEST_F(IndexOperationsTest, CanDisableAndEnableIndex)
	{
		auto store = get_document_store(TEST_NAME);
		Users_Index().execute(store);

		store->maintenance()->send(
			ravendb::client::documents::operations::indexes::DisableIndexOperation(Users_Index().get_index_name()));

		auto indexing_status = store->maintenance()->send(ravendb::client::documents::operations::indexes::GetIndexingStatusOperation());

		auto index_status = indexing_status->indexes[0];
		ASSERT_EQ(ravendb::client::documents::indexes::IndexRunningStatus::DISABLED, index_status.status);

		store->maintenance()->send(
			ravendb::client::documents::operations::indexes::EnableIndexOperation(Users_Index().get_index_name()));

		indexing_status = store->maintenance()->send(ravendb::client::documents::operations::indexes::GetIndexingStatusOperation());
		ASSERT_EQ(ravendb::client::documents::indexes::IndexRunningStatus::RUNNING, indexing_status->indexes[0].status);
	}

	TEST_F(IndexOperationsTest, CanGetIndexes)
	{
		auto store = get_document_store(TEST_NAME);
		Users_Index().execute(store);

		auto index_definitions = store->maintenance()->send(
			ravendb::client::documents::operations::indexes::GetIndexesOperation(0, 10));
		ASSERT_EQ(1, index_definitions->size());
	}

	TEST_F(IndexOperationsTest, CanGetIndexesStats)
	{
		auto store = get_document_store(TEST_NAME);
		Users_Index().execute(store);

		auto indexes_stats = store->maintenance()->send(
			ravendb::client::documents::operations::indexes::GetIndexesStatisticsOperation());

		ASSERT_EQ(1, indexes_stats->size());
	}

	TEST_F(IndexOperationsTest, CanGetTerms)
	{
		auto store = get_document_store(TEST_NAME);
		Users_Index().execute(store);

		{
			auto session = store->open_session();
			auto user = std::make_shared<infrastructure::entities::User>();
			user->name = "Alexander";
			session.store(user);
			session.save_changes();
		}

		wait_for_indexing(store, store->get_database());

		auto terms = store->maintenance()->send(
			ravendb::client::documents::operations::indexes::GetTermsOperation(Users_Index().get_index_name(), "name"));

		ASSERT_EQ(1, terms->size());
		ASSERT_NE(std::find(terms->cbegin(), terms->cend(), "alexander"), terms->cend());
	}

	TEST_F(IndexOperationsTest, HasIndexChanged)
	{
		auto store = get_document_store(TEST_NAME);
		auto index = Users_Index();
		auto index_def = index.create_index_definition();

		store->maintenance()->send(ravendb::client::documents::operations::indexes::PutIndexesOperation({ index_def }));

		ASSERT_FALSE(*store->maintenance()->send(
			ravendb::client::documents::operations::indexes::IndexHasChangedOperation(index_def)));

		index_def.maps = { "from users" };
		ASSERT_TRUE(*store->maintenance()->send(
			ravendb::client::documents::operations::indexes::IndexHasChangedOperation(index_def)));
	}

	TEST_F(IndexOperationsTest, CanStopStartIndexing)
	{
		auto store = get_document_store(TEST_NAME);
		auto index = Users_Index();
		auto index_def = index.create_index_definition();

		store->maintenance()->send(ravendb::client::documents::operations::indexes::PutIndexesOperation({ index_def }));

		store->maintenance()->send(ravendb::client::documents::operations::indexes::StopIndexingOperation());

		auto indexing_status = store->maintenance()->send(
			ravendb::client::documents::operations::indexes::GetIndexingStatusOperation());

		ASSERT_EQ(ravendb::client::documents::indexes::IndexRunningStatus::PAUSED, indexing_status->status);

		store->maintenance()->send(ravendb::client::documents::operations::indexes::StartIndexingOperation());

		indexing_status = store->maintenance()->send(
			ravendb::client::documents::operations::indexes::GetIndexingStatusOperation());

		ASSERT_EQ(ravendb::client::documents::indexes::IndexRunningStatus::RUNNING, indexing_status->status);
	}

	TEST_F(IndexOperationsTest, CanStopStartIndex)
	{
		auto store = get_document_store(TEST_NAME);
		auto index = Users_Index();
		auto index_def = index.create_index_definition();

		store->maintenance()->send(ravendb::client::documents::operations::indexes::PutIndexesOperation({ index_def }));

		store->maintenance()->send(
			ravendb::client::documents::operations::indexes::StopIndexOperation(index_def.name));

		auto indexing_status = store->maintenance()->send(
			ravendb::client::documents::operations::indexes::GetIndexingStatusOperation());

		ASSERT_EQ(ravendb::client::documents::indexes::IndexRunningStatus::RUNNING, indexing_status->status);
		ASSERT_EQ(ravendb::client::documents::indexes::IndexRunningStatus::PAUSED, indexing_status->indexes[0].status);

		store->maintenance()->send(
			ravendb::client::documents::operations::indexes::StartIndexOperation(index_def.name));

		indexing_status = store->maintenance()->send(
			ravendb::client::documents::operations::indexes::GetIndexingStatusOperation());

		ASSERT_EQ(ravendb::client::documents::indexes::IndexRunningStatus::RUNNING, indexing_status->status);
		ASSERT_EQ(ravendb::client::documents::indexes::IndexRunningStatus::RUNNING, indexing_status->indexes[0].status);
	}

	TEST_F(IndexOperationsTest, CanSetIndexLockMode)
	{
		auto store = get_document_store(TEST_NAME);
		auto index = Users_Index();
		auto index_def = index.create_index_definition();

		store->maintenance()->send(ravendb::client::documents::operations::indexes::PutIndexesOperation({ index_def }));

		store->maintenance()->send(
			ravendb::client::documents::operations::indexes::SetIndexesLockOperation(index_def.name, ravendb::client::documents::indexes::IndexLockMode::LOCKED_ERROR));

		auto new_index_def = store->maintenance()->send(
			ravendb::client::documents::operations::indexes::GetIndexOperation(index_def.name));

		ASSERT_EQ(ravendb::client::documents::indexes::IndexLockMode::LOCKED_ERROR, new_index_def->lock_mode);
	}

	TEST_F(IndexOperationsTest, CanSetIndexPriority)
	{
		auto store = get_document_store(TEST_NAME);
		auto index = Users_Index();
		auto index_def = index.create_index_definition();

		store->maintenance()->send(ravendb::client::documents::operations::indexes::PutIndexesOperation({ index_def }));

		store->maintenance()->send(
			ravendb::client::documents::operations::indexes::SetIndexesPriorityOperation(index_def.name, ravendb::client::documents::indexes::IndexPriority::HIGH));

		auto new_index_def = store->maintenance()->send(
			ravendb::client::documents::operations::indexes::GetIndexOperation(index_def.name));

		ASSERT_EQ(ravendb::client::documents::indexes::IndexPriority::HIGH, new_index_def->priority);
	}

	TEST_F(IndexOperationsTest, CanListErrors)
	{
		auto store = get_document_store(TEST_NAME);
		auto index = UsersInvalidIndex();
		auto index_def = index.create_index_definition();

		store->maintenance()->send(ravendb::client::documents::operations::indexes::PutIndexesOperation({ index_def }));

		{
			auto session = store->open_session();
			auto user = std::make_shared<infrastructure::entities::User>();
			user->name = "";
			user->age = 0;
			session.store(user);
			session.save_changes();
		}

		wait_for_indexing(store);

		//Spin wait for index to change it's state
		const std::chrono::milliseconds wait_timeout = std::chrono::seconds(20);
		impl::SimpleStopWatch sp{};
		while (sp.millis_elapsed() < wait_timeout)
		{
			auto database_statistics = store->maintenance()->send(
				ravendb::client::documents::operations::GetStatisticsOperation());
			if(database_statistics->indexes[0].state == ravendb::client::documents::indexes::IndexState::ERRONEOUS)
			{
				break;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		//just in case ...
        std::this_thread::sleep_for(std::chrono::milliseconds(300));

		auto index_errors = store->maintenance()->send(
			ravendb::client::documents::operations::indexes::GetIndexErrorsOperation());
		auto per_index_errors = store->maintenance()->send(
			ravendb::client::documents::operations::indexes::GetIndexErrorsOperation({ index_def.name }));

		ASSERT_EQ(index_def.name, (*index_errors)[0].name);
		ASSERT_EQ(1, (*index_errors)[0].errors.size());
		
		ASSERT_EQ(index_def.name, (*per_index_errors)[0].name);
		ASSERT_EQ(1, (*per_index_errors)[0].errors.size());
	}

	TEST_F(IndexOperationsTest, CanGetIndexStatistics)
	{
		auto store = get_document_store(TEST_NAME);
		auto index = Users_Index();
		auto index_def = index.create_index_definition();

		store->maintenance()->send(ravendb::client::documents::operations::indexes::PutIndexesOperation({ index_def }));

		{
			auto session = store->open_session();
			auto user = std::make_shared<infrastructure::entities::User>();
			user->name = "";
			user->age = 0;
			session.store(user);
			session.save_changes();
		}

		wait_for_indexing(store);

		auto stats = store->maintenance()->send(
			ravendb::client::documents::operations::indexes::GetIndexStatisticsOperation(index_def.name));
		ASSERT_EQ(1, stats->entries_count);
	}
}