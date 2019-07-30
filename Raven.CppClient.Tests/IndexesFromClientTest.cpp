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
#include "GetTermsOperation.h"
#include "ExplainQueryCommand.h"
#include "Post.h"

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
			REGISTER_ID_PROPERTY_FOR(ravendb::client::tests::infrastructure::entities::Post, id);
		}

		class Users_ByName : public documents::indexes::AbstractIndexCreationTask
		{
		public:
			~Users_ByName() override = default;
			Users_ByName()
			{
				SET_DEFAULT_INDEX_NAME();

				map = "from u in docs.Users select new { u.name }";

				index("name", documents::indexes::FieldIndexing::SEARCH);

				index_suggestions.insert("name");

				store("name", documents::indexes::FieldStorage::YES);
			}
		};

		class UsersIndex : public documents::indexes::AbstractIndexCreationTask
		{
		public:
			~UsersIndex() override = default;
			UsersIndex()
			{
				SET_DEFAULT_INDEX_NAME();

				map = "from user in docs.users select new { user.Name }";
			}
		};

		class Posts_ByTitleAndDesc : public documents::indexes::AbstractIndexCreationTask
		{
		public:
			~Posts_ByTitleAndDesc() override = default;
			Posts_ByTitleAndDesc()
			{
				SET_DEFAULT_INDEX_NAME();

				map = "from p in docs.Posts select new { p.title, p.desc }";
				using namespace ravendb::client::documents::indexes;
				index("title", FieldIndexing::SEARCH);
				store("title", FieldStorage::YES);
				analyze("title", "Lucene.Net.Analysis.SimpleAnalyzer");

				index("desc", FieldIndexing::SEARCH);
				store("desc", FieldStorage::YES);
				analyze("desc", "Lucene.Net.Analysis.SimpleAnalyzer");
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
			auto users = session.query<infrastructure::entities::User, Users_ByName>()
				->to_list();
			ASSERT_EQ(1, users.size());
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

		ASSERT_EQ(documents::indexes::IndexRunningStatus::RUNNING, status->status);
		ASSERT_EQ(1, status->indexes.size());
		ASSERT_EQ(documents::indexes::IndexRunningStatus::RUNNING, status->indexes[0].status);

		store->maintenance()->send(documents::operations::indexes::StopIndexingOperation());

		status = store->maintenance()->send(documents::operations::indexes::GetIndexingStatusOperation());

		ASSERT_EQ(documents::indexes::IndexRunningStatus::PAUSED, status->status);
		ASSERT_EQ(documents::indexes::IndexRunningStatus::PAUSED, status->indexes[0].status);

		store->maintenance()->send(documents::operations::indexes::StartIndexingOperation());

		status = store->maintenance()->send(documents::operations::indexes::GetIndexingStatusOperation());

		ASSERT_EQ(documents::indexes::IndexRunningStatus::RUNNING, status->status);
		ASSERT_EQ(1, status->indexes.size());
		ASSERT_EQ(documents::indexes::IndexRunningStatus::RUNNING, status->indexes[0].status);

		store->maintenance()->send(documents::operations::indexes::StopIndexOperation(status->indexes[0].name));

		status = store->maintenance()->send(documents::operations::indexes::GetIndexingStatusOperation());

		ASSERT_EQ(documents::indexes::IndexRunningStatus::RUNNING, status->status);
		ASSERT_EQ(1, status->indexes.size());
		ASSERT_EQ(documents::indexes::IndexRunningStatus::PAUSED, status->indexes[0].status);
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
		{
			auto session = store->open_session();

			auto users = session.query<infrastructure::entities::User, Users_ByName>()
				->wait_for_non_stale_results()
				->where_equals("name", "Alexey")
				->to_list();

			ASSERT_EQ(1, users.size());
		}

		auto indexes = store->maintenance()->send(documents::operations::indexes::GetIndexesOperation(0, 100));
		ASSERT_EQ(1, indexes->size());

		auto&& index = (*indexes)[0];
		auto stats = store->maintenance()->send(documents::operations::indexes::GetIndexStatisticsOperation(index.name));

		ASSERT_EQ(documents::indexes::IndexLockMode::UNLOCK, stats->lock_mode);
		ASSERT_EQ(documents::indexes::IndexPriority::NORMAL, stats->priority);

		store->maintenance()->send(documents::operations::indexes::SetIndexesLockOperation(index.name,
			documents::indexes::IndexLockMode::LOCKED_IGNORE));
		store->maintenance()->send(documents::operations::indexes::SetIndexesPriorityOperation(index.name,
		documents::indexes::IndexPriority::LOW));

		stats = store->maintenance()->send(documents::operations::indexes::GetIndexStatisticsOperation(index.name));

		ASSERT_EQ(documents::indexes::IndexLockMode::LOCKED_IGNORE, stats->lock_mode);
		ASSERT_EQ(documents::indexes::IndexPriority::LOW, stats->priority);
	}

	TEST_F(IndexesFromClientTest, GetTerms)
	{
		auto store = get_document_store(TEST_NAME);
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

		std::string index_name{};

		{
			auto session = store->open_session();
			std::shared_ptr<documents::session::QueryStatistics> stats_ref{};
			auto users = session.query<infrastructure::entities::User>()
				->wait_for_non_stale_results()
				->statistics(stats_ref)
				->where_equals("name", "Alexey")
				->to_list();

			index_name = stats_ref->index_name;
		}

		auto terms = store->maintenance()->send(documents::operations::indexes::GetTermsOperation(
			index_name, "name", {}, 128));

		ASSERT_EQ(2, terms->size());

		ASSERT_TRUE(std::find(terms->begin(), terms->end(), "alexander") != terms->end());
		ASSERT_TRUE(std::find(terms->begin(), terms->end(), "alexey") != terms->end());
	}

	TEST_F(IndexesFromClientTest, GetIndexNames)
	{
		auto store = get_document_store(TEST_NAME);
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

		std::string index_name{};

		{
			auto session = store->open_session();
			std::shared_ptr<documents::session::QueryStatistics> stats_ref{};
			auto users = session.query<infrastructure::entities::User>()
				->wait_for_non_stale_results()
				->statistics(stats_ref)
				->where_equals("name", "Alexey")
				->to_list();

			index_name = stats_ref->index_name;
		}

		auto index_names = store->maintenance()->send(documents::operations::indexes::GetIndexNamesOperation(0, 128));

		ASSERT_EQ(1, index_names->size());

		ASSERT_TRUE(std::find(index_names->begin(), index_names->end(), index_name) != index_names->end());
	}

	TEST_F(IndexesFromClientTest, CanExplain)
	{
		auto store = get_document_store(TEST_NAME);
		auto user1 = std::make_shared<infrastructure::entities::User>();
		user1->name = "Alexander";

		auto user2 = std::make_shared<infrastructure::entities::User>();
		user2->name = "Alexey";

		{
			auto session = store->open_session();
			session.store(user1);
			session.store(user2);
			session.save_changes();
		}
		{
			auto session = store->open_session();

			std::shared_ptr<documents::session::QueryStatistics> stats_ref{};
			auto users = session.query<infrastructure::entities::User>()
				->statistics(stats_ref)
				->where_equals("name", "Alexey")
				->to_list();

			users = session.query<infrastructure::entities::User>()
				->statistics(stats_ref)
				->where_greater_than("age", 10)
				->to_list();
		}

		auto index_query = documents::queries::IndexQuery("from users");
		auto command = documents::commands::ExplainQueryCommand(store->get_conventions(), index_query);
		store->get_request_executor()->execute(command);

		auto explanations = command.get_result();

		ASSERT_EQ(1, explanations->size());
		ASSERT_FALSE(explanations->at(0).index.empty());
		ASSERT_FALSE(explanations->at(0).reason.empty());
	}

	//TODO waiting for moreLikeThis implementation
	TEST_F(IndexesFromClientTest, DISABLED_MoreLikeThis)
	{
		auto store = get_document_store(TEST_NAME);

		{
			auto session = store->open_session();

			auto post1 = std::make_shared<infrastructure::entities::Post>();
			post1->id = "posts/1";
			post1->title = "doduck";
			post1->desc = "prototype";
			session.store(post1);

			auto post2 = std::make_shared<infrastructure::entities::Post>();
			post2->id = "posts/2";
			post2->title = "doduck";
			post2->desc = "prototype your idea";
			session.store(post2);

			auto post3 = std::make_shared<infrastructure::entities::Post>();
			post3->id = "posts/3";
			post3->title = "doduck";
			post3->desc = "love programming";
			session.store(post3);

			auto post4 = std::make_shared<infrastructure::entities::Post>();
			post4->id = "posts/4";
			post4->title = "We do";
			post4->desc = "prototype";
			session.store(post4);

			auto post5 = std::make_shared<infrastructure::entities::Post>();
			post5->id = "posts/5";
			post5->title = "We love";
			post5->desc = "challange";
			session.store(post5);

			session.save_changes();
		}

		Posts_ByTitleAndDesc().execute(store);
		wait_for_indexing(store);

		{
			auto session = store->open_session();

			auto options = documents::queries::more_like_this::MoreLikeThisOptions();
			options.minimum_document_frequency = 1;
			options.minimum_term_frequency = 0;


		}
	}
}
