#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "User.h"
#include "EntityIdHelperUtil.h"
#include "AbstractIndexCreationTask.h"
#include "IndexCreation.h"
#include "DocumentSession.h"
#include "GetStatisticsOperation.h"
#include "MaintenanceOperationExecutor.h"
#include "ResetIndexOperation.h"
#include "GetIndexNamesOperation.h"
#include "DeleteIndexOperation.h"
#include "GetIndexingStatusOperation.h"
#include "StopIndexingOperation.h"
#include "StartIndexingOperationh.h"
#include "StopIndexOperation.h"
#include "GetIndexesOperation.h"
#include "GetIndexStatisticsOperation.h"
#include "SetIndexesLockOperation.h"
#include "SetIndexesPriorityOperation.h"

namespace ravendb::client::tests::client::indexing
{
	class IndexesFromClientTest : public driver::RavenTestDriver
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

		class Users_ByName : public documents::indexes::AbstractIndexCreationTask
		{
		public:
			~Users_ByName() override = default;
			Users_ByName()
			{
				SET_DEFAULT_INDEX_NAME;

				map = "from u in docs.Users select new { u.Name }";

				index("Name", documents::indexes::FieldIndexing::SEARCH);

				index_suggestions.insert("name");

				store("Name", documents::indexes::FieldStorage::YES);
			}
		};

		class UsersIndex : public documents::indexes::AbstractIndexCreationTask
		{
		public:
			~UsersIndex() override = default;
			UsersIndex()
			{
				SET_DEFAULT_INDEX_NAME;

				map = "from user in docs.users select new { user.Name }";
			}
		};
	};

	TEST_F(IndexesFromClientTest, CanCreateIndexesUsingIndexCreation)
	{
		auto store = get_document_store(TEST_NAME);

		auto users_by_name = std::vector<std::shared_ptr<documents::indexes::AbstractIndexCreationTask>>
			{ std::make_shared<Users_ByName>() };
		documents::indexes::IndexCreation::create_indexes(users_by_name, store);

		{
			auto session = store->open_session();
			auto user1 = std::make_shared<infrastructure::entities::User>();
			user1->name = "Alexander";
			session.store(user1, "users/1");
			session.save_changes();
		}

		wait_for_indexing(store);

		{
			auto session = store->open_session();
			//TODO
			//auto users = session.query<infrastructure::entities::User, Users_ByName>();
			//ASSERT_EQ(1, users.size());
		}
	}

	TEST_F(IndexesFromClientTest, CanReset)
	{
		auto store = get_document_store(TEST_NAME);

		{
			auto session = store->open_session();
			auto user1 = std::make_shared<infrastructure::entities::User>();
			user1->name = "Alexander";
			session.store(user1, "users/1");
			session.save_changes();
		}

		store->execute_index(std::make_shared<UsersIndex>());

		wait_for_indexing(store);

		auto command = documents::operations::GetStatisticsOperation().get_command(store->get_conventions());
		store->get_request_executor()->execute(*command);

		auto statistics = command->get_result();
		auto first_indexing_time = statistics->indexes[0].last_indexing_time;

		auto index_name = UsersIndex().get_index_name();

		// now reset index

		std::this_thread::sleep_for(std::chrono::milliseconds(2));// avoid the same millisecond

		store->maintenance()->send(documents::operations::indexes::ResetIndexOperation(index_name));
		wait_for_indexing(store);

		command = documents::operations::GetStatisticsOperation().get_command(store->get_conventions());
		store->get_request_executor()->execute(*command);
		statistics = command->get_result();

		auto second_indexing_time = statistics->last_indexing_time;
		ASSERT_GT(*second_indexing_time, first_indexing_time);
	}

	TEST_F(IndexesFromClientTest, CanExecuteManyIndexes)
	{
		auto store = get_document_store(TEST_NAME);

		auto indexes = std::vector<std::shared_ptr<documents::indexes::AbstractIndexCreationTaskBase>>{
		std::make_shared<UsersIndex>(), std::make_shared<Users_ByName>() };

		store->execute_indexes(indexes);

		wait_for_indexing(store);

		auto index_names_operation = documents::operations::indexes::GetIndexNamesOperation(0, 100);
		auto indexes_names = store->maintenance()->send(index_names_operation);

		ASSERT_EQ(2, indexes_names->size());
	}


	TEST_F(IndexesFromClientTest, CanDelete)
	{
		auto store = get_document_store(TEST_NAME);

		auto new_index = std::make_shared<UsersIndex>();
		store->execute_index(new_index);
		wait_for_indexing(store);

		store->maintenance()->send(documents::operations::indexes::DeleteIndexOperation(new_index->get_index_name()));

		auto command = documents::operations::GetStatisticsOperation().get_command(store->get_conventions());
		store->get_request_executor()->execute(*command);

		auto statistics = command->get_result();
		ASSERT_TRUE(statistics->indexes.empty());
	}

	TEST_F(IndexesFromClientTest, CanStopAndStart)
	{
		auto store = get_document_store(TEST_NAME);

		Users_ByName().execute(store);

		auto status = store->maintenance()->send(documents::operations::indexes::GetIndexingStatusOperation());

		ASSERT_EQ(IndexRunningStatus::RUNNING, status->status);
		ASSERT_EQ(1, status->indexes.size());
		ASSERT_EQ(IndexRunningStatus::RUNNING, status->indexes[0].status);

		store->maintenance()->send(documents::operations::indexes::StopIndexingOperation());

		status = store->maintenance()->send(documents::operations::indexes::GetIndexingStatusOperation());

		ASSERT_EQ(IndexRunningStatus::PAUSED, status->status);
		ASSERT_EQ(IndexRunningStatus::PAUSED, status->indexes[0].status);

		store->maintenance()->send(documents::operations::indexes::StartIndexingOperation());

		status = store->maintenance()->send(documents::operations::indexes::GetIndexingStatusOperation());

		ASSERT_EQ(IndexRunningStatus::RUNNING, status->status);
		ASSERT_EQ(1, status->indexes.size());
		ASSERT_EQ(IndexRunningStatus::RUNNING, status->indexes[0].status);

		store->maintenance()->send(documents::operations::indexes::StopIndexOperation(status->indexes[0].name));

		status = store->maintenance()->send(documents::operations::indexes::GetIndexingStatusOperation());

		ASSERT_EQ(IndexRunningStatus::RUNNING, status->status);
		ASSERT_EQ(1, status->indexes.size());
		ASSERT_EQ(IndexRunningStatus::PAUSED, status->indexes[0].status);
	}

	TEST_F(IndexesFromClientTest, SetLockModeAndSetPriority)
	{
		auto store = get_document_store(TEST_NAME);

		Users_ByName().execute(store);

		{
			auto session = store->open_session();
			auto user1 = std::make_shared<infrastructure::entities::User>();
			user1->name = "Alexander";
			session.store(user1);

			auto user2 = std::make_shared<infrastructure::entities::User>();
			user2->name = "Alexey";
			session.store(user2);

			session.save_changes();
		}

		//TODO
		//try (IDocumentSession session = store.openSession()) {
		//	List<User> users = session
		//		.query(User.class, Users_ByName.class)
		//		.waitForNonStaleResults()
		//		.whereEquals("name", "Arek")
		//		.toList();


		//	assertThat(users)
		//		.hasSize(1);
		//}
		wait_for_indexing(store);

		auto indexes = store->maintenance()->send(documents::operations::indexes::GetIndexesOperation(0, 100));
		ASSERT_EQ(1, indexes->size());

		auto&& index = (*indexes)[0];
		auto stats = store->maintenance()->send(documents::operations::indexes::GetIndexStatisticsOperation(index.name));

		ASSERT_EQ(documents::indexes::IndexLockMode::UNLOCK, stats->lock_mode);
		ASSERT_EQ(documents::indexes::IndexPriority::NORMAL, stats->priority);

		store->maintenance()->send(documents::operations::indexes::SetIndexesLockOperation(index.name,
			IndexLockMode::LOCKED_IGNORE));
		store->maintenance()->send(documents::operations::indexes::SetIndexesPriorityOperation(index.name,
			IndexPriority::LOW));

		stats = store->maintenance()->send(documents::operations::indexes::GetIndexStatisticsOperation(index.name));

		ASSERT_EQ(documents::indexes::IndexLockMode::LOCKED_IGNORE, stats->lock_mode);
		ASSERT_EQ(documents::indexes::IndexPriority::LOW, stats->priority);
	}

	//TODO implement | waiting for queries
	TEST_F(IndexesFromClientTest, GetTerms)
	{
		auto store = get_document_store(TEST_NAME);
	}

	//TODO implement | waiting for queries
	TEST_F(IndexesFromClientTest, GetIndexNames)
	{
		auto store = get_document_store(TEST_NAME);
	}

	//TODO implement | waiting for queries
	TEST_F(IndexesFromClientTest, CanExplain)
	{
		auto store = get_document_store(TEST_NAME);
	}

	//TODO implement | waiting for query/moreLikeThis
	TEST_F(IndexesFromClientTest, MoreLikeThis)
	{
		auto store = get_document_store(TEST_NAME);
	}
}