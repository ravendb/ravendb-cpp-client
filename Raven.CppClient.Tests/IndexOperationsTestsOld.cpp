#include "pch.h"
//#define __USE_FIDDLER__
#include "re_definitions.h"

#include "User.h"
#include "GetDocumentsCommand.h"
#include "PutDocumentCommand.h"
#include "DeleteDocumentCommand.h"

#include "GetIndexOperation.h"
#include "PutIndexesOperation.h"
#include "DeleteIndexOperation.h"
#include "GetIndexNamesOperation.h"
#include "GetIndexingStatusOperation.h"
#include "DisableIndexOperation.h"
#include "EnableIndexOperation.h"
#include "GetIndexesOperation.h"
#include "GetIndexesStatisticsOperation.h"
#include "GetTermsOperation.h"
#include "IndexHasChangedOperation.h"
#include "StopIndexingOperation.h"
#include "StartIndexingOperationh.h"
#include "StopIndexOperation.h"
#include "StartIndexOperation.h"
#include "SetIndexesLockOperation.h"
#include "SetIndexesPriorityOperation.h"
#include "GetIndexErrorsOperation.h"


namespace ravendb::client::tests::old_tests
{
	class IndexOperationsTestOld : public ::testing::Test
	{
	protected:
		inline static std::shared_ptr<definitions::RequestExecutorScope> test_suite_executor{};

		static const infrastructure::entities::User example_user;
		static documents::indexes::IndexDefinition example_index;

		static void SetUpTestCase()
		{
			test_suite_executor = definitions::GET_REQUEST_EXECUTOR();
			set_example_index();
		}
		static void TearDownTestCase()
		{
			test_suite_executor.reset();
		}

		void SetUp() override//create sample document and sample index
		{
			nlohmann::json j = example_user;
			j["@metadata"]["@collection"] = "Users";
			documents::commands::PutDocumentCommand cmd(example_user.id, {}, j);
			test_suite_executor->get().execute(cmd);

			auto op = documents::operations::indexes::PutIndexesOperation({ example_index });
			test_suite_executor->get().execute(*op.get_command({}));

			//waiting for indexing TODO something better
			std::this_thread::sleep_for(std::chrono::duration<int>(3));
		}

		void TearDown() override //delete all documents and all indexes
		{
			auto op1 = documents::operations::indexes::GetIndexNamesOperation(0, 100);
			auto cmd1 = op1.get_command({});
			test_suite_executor->get().execute(*cmd1);
			auto&& res = cmd1->get_result();
			for (auto& index_name : *res)
			{
				auto op = documents::operations::indexes::DeleteIndexOperation(index_name);
				test_suite_executor->get().execute(*op.get_command({}));
			}
			
			auto cmd = documents::commands::GetDocumentsCommand(0, 100);
			test_suite_executor->get().execute(cmd);
			auto res1 = cmd.get_result();
			for(auto& result : res1->results)
			{
				auto u = result.get<infrastructure::entities::User>();
				documents::commands::DeleteDocumentCommand cmd2(result["@metadata"]["@id"].get<std::string>());
				test_suite_executor->get().execute(cmd2);
			}
		}

		bool does_index_exist_by_get_index_op(const std::string& index_name)
		{
			auto op = documents::operations::indexes::GetIndexOperation(index_name);
			try
			{
				auto cmd = op.get_command({});
				test_suite_executor->get().execute(*cmd);
			}catch (exceptions::RavenException&)
			{
				return false;
			}
			return true;
		}

		bool does_index_exist_by_get_index_names_op(const std::string& index_name)
		{
			auto op = documents::operations::indexes::GetIndexNamesOperation(0, 10);
			auto cmd = op.get_command({});
			test_suite_executor->get().execute(*cmd);
			auto&& res = cmd->get_result();
			return std::find(res->cbegin(), res->cend(), index_name) != res->cend();
		}

		static void set_example_index()
		{
			example_index.name = "UsersIndex";
			example_index.maps = { R"(
				from user in docs.Users 
				select new 
				{
					fullName = user.name + " " + user.lastName
				})"};
		}
	};
	const infrastructure::entities::User IndexOperationsTestOld::example_user{ "Users/1", "Alexander", "Timoshenko", "Israel", 0, 38 };
	documents::indexes::IndexDefinition IndexOperationsTestOld::example_index{};
	
	TEST_F(IndexOperationsTestOld, CanGetIndex)
	{
		ASSERT_TRUE(does_index_exist_by_get_index_op(example_index.name));

		auto op = documents::operations::indexes::GetIndexOperation(example_index.name);
		auto cmd = op.get_command({});
		test_suite_executor->get().execute(*cmd);
		auto res = cmd->get_result();

		const documents::indexes::IndexDefinition check_index =
		{
			example_index.name,
			documents::indexes::IndexPriority::NORMAL,
			documents::indexes::IndexLockMode::UNLOCK,{},
			example_index.maps,{},{},{},
			documents::indexes::IndexType::MAP,{}
		};
		ASSERT_TRUE(check_index == *res);
	}

	TEST_F(IndexOperationsTestOld, CanDeleteIndex)
	{
		ASSERT_TRUE(does_index_exist_by_get_index_names_op(example_index.name));

		auto op = documents::operations::indexes::DeleteIndexOperation(example_index.name);
		auto cmd = op.get_command({});
		test_suite_executor->get().execute(*cmd);
		auto&& res = cmd->get_result();

		ASSERT_FALSE(does_index_exist_by_get_index_names_op(example_index.name));
	}

	TEST_F(IndexOperationsTestOld, CanDisableAndEnableIndex)
	{
		ASSERT_TRUE(does_index_exist_by_get_index_op(example_index.name));
		{
			auto op = documents::operations::indexes::GetIndexingStatusOperation();
			auto cmd = op.get_command({});
			test_suite_executor->get().execute(*cmd);
			auto&& res = cmd->get_result();
			EXPECT_EQ(res->indexes[0].status, documents::indexes::IndexRunningStatus::RUNNING);
		}
		{
			auto op = documents::operations::indexes::DisableIndexOperation(example_index.name);
			auto cmd = op.get_command({});
			test_suite_executor->get().execute(*cmd);
		}
		{
			auto op = documents::operations::indexes::GetIndexingStatusOperation();
			auto cmd = op.get_command({});
			test_suite_executor->get().execute(*cmd);
			auto&& res = cmd->get_result();
			ASSERT_EQ(res->indexes[0].status, documents::indexes::IndexRunningStatus::DISABLED);
		}

		{
			auto op = documents::operations::indexes::EnableIndexOperation(example_index.name);
			auto cmd = op.get_command({});
			test_suite_executor->get().execute(*cmd);
		}
		{
			auto op = documents::operations::indexes::GetIndexingStatusOperation();
			auto cmd = op.get_command({});
			test_suite_executor->get().execute(*cmd);
			auto&& res = cmd->get_result();
			ASSERT_EQ(res->indexes[0].status, documents::indexes::IndexRunningStatus::RUNNING);
		}
	}

	TEST_F(IndexOperationsTestOld, CanGetIndexes)
	{
		ASSERT_TRUE(does_index_exist_by_get_index_op(example_index.name));

		auto op = documents::operations::indexes::GetIndexesOperation(0,10);
		auto cmd = op.get_command({});
		test_suite_executor->get().execute(*cmd);
		auto&& res = cmd->get_result();

		ASSERT_EQ(res->size(), 1);
		ASSERT_EQ((*res)[0].name, example_index.name);
	}

	TEST_F(IndexOperationsTestOld, CanGetIndexesStats)
	{
		ASSERT_TRUE(does_index_exist_by_get_index_op(example_index.name));

		auto op = documents::operations::indexes::GetIndexesStatisticsOperation();
		auto cmd = op.get_command({});
		test_suite_executor->get().execute(*cmd);
		auto&& res = cmd->get_result();

		ASSERT_EQ(res->size(), 1);
		ASSERT_EQ((*res)[0].name, example_index.name);
	}

	TEST_F(IndexOperationsTestOld, CanGetTerms)
	{
		ASSERT_TRUE(does_index_exist_by_get_index_op(example_index.name));

		auto op = documents::operations::indexes::GetTermsOperation(example_index.name,"fullName");
		auto cmd = op.get_command({});
		test_suite_executor->get().execute(*cmd);
		auto&& res = cmd->get_result();

		ASSERT_EQ(res->size(), 1);
		std::string expected_str = example_user.name + ' ' + example_user.lastName;
		std::transform(expected_str.cbegin(), expected_str.cend(), expected_str.begin(),
			[](std::string::value_type c) {return std::tolower(c); });
		ASSERT_EQ((*res)[0], expected_str);
	}

	TEST_F(IndexOperationsTestOld, HasIndexChanged)
	{
		ASSERT_TRUE(does_index_exist_by_get_index_op(example_index.name));
		{
			auto op = documents::operations::indexes::IndexHasChangedOperation(example_index);
			auto cmd = op.get_command({});
			test_suite_executor->get().execute(*cmd);
			auto&& res = cmd->get_result();

			ASSERT_FALSE(*res);
		}
		auto changed_index = example_index;
		changed_index.maps = { R"(
				from user in docs.Users 
				select new 
				{
					fullName = user.name + ' ' + user.lastName + " from " + user.addressId
				})" };
		{
			auto op = documents::operations::indexes::IndexHasChangedOperation(changed_index);
			auto cmd = op.get_command({});
			test_suite_executor->get().execute(*cmd);
			auto&& res = cmd->get_result();

			ASSERT_TRUE(*res);
		}
	}

	TEST_F(IndexOperationsTestOld, CanStopStartIndexing)
	{
		ASSERT_TRUE(does_index_exist_by_get_index_op(example_index.name));
		{
			auto op = documents::operations::indexes::GetIndexingStatusOperation();
			auto cmd = op.get_command({});
			test_suite_executor->get().execute(*cmd);
			auto&& res = cmd->get_result();
			EXPECT_EQ(res->status, documents::indexes::IndexRunningStatus::RUNNING);
		}
		{
			auto op = documents::operations::indexes::StopIndexingOperation();
			auto cmd = op.get_command({});
			test_suite_executor->get().execute(*cmd);
			auto&& res = cmd->get_result();
		}
		{
			auto op = documents::operations::indexes::GetIndexingStatusOperation();
			auto cmd = op.get_command({});
			test_suite_executor->get().execute(*cmd);
			auto&& res = cmd->get_result();
			ASSERT_EQ(res->status, documents::indexes::IndexRunningStatus::PAUSED);
		}

		{
			auto op = documents::operations::indexes::StartIndexingOperation();
			auto cmd = op.get_command({});
			test_suite_executor->get().execute(*cmd);
		}
		{
			auto op = documents::operations::indexes::GetIndexingStatusOperation();
			auto cmd = op.get_command({});
			test_suite_executor->get().execute(*cmd);
			auto&& res = cmd->get_result();
			ASSERT_EQ(res->status, documents::indexes::IndexRunningStatus::RUNNING);
		}
	}

	TEST_F(IndexOperationsTestOld, CanStopStartIndex)
	{
		ASSERT_TRUE(does_index_exist_by_get_index_op(example_index.name));
		{
			auto op = documents::operations::indexes::GetIndexingStatusOperation();
			auto cmd = op.get_command({});
			test_suite_executor->get().execute(*cmd);
			auto&& res = cmd->get_result();
			EXPECT_EQ(res->status, documents::indexes::IndexRunningStatus::RUNNING);
			EXPECT_EQ(res->indexes[0].status, documents::indexes::IndexRunningStatus::RUNNING);
		}
		{
			auto op = documents::operations::indexes::StopIndexOperation(example_index.name);
			auto cmd = op.get_command({});
			test_suite_executor->get().execute(*cmd);
		}
		{
			auto op = documents::operations::indexes::GetIndexingStatusOperation();
			auto cmd = op.get_command({});
			test_suite_executor->get().execute(*cmd);
			auto&& res = cmd->get_result();
			ASSERT_EQ(res->status, documents::indexes::IndexRunningStatus::RUNNING);
			ASSERT_EQ(res->indexes[0].status, documents::indexes::IndexRunningStatus::PAUSED);
		}

		{
			auto op = documents::operations::indexes::StartIndexOperation(example_index.name);
			auto cmd = op.get_command({});
			test_suite_executor->get().execute(*cmd);
		}
		{
			auto op = documents::operations::indexes::GetIndexingStatusOperation();
			auto cmd = op.get_command({});
			test_suite_executor->get().execute(*cmd);
			auto&& res = cmd->get_result();
			ASSERT_EQ(res->status, documents::indexes::IndexRunningStatus::RUNNING);
			ASSERT_EQ(res->indexes[0].status, documents::indexes::IndexRunningStatus::RUNNING);
		}
	}

	TEST_F(IndexOperationsTestOld, CanSetIndexLockMode)
	{
		//should NOT use IndexLockMode::UNSET for SetIndexesLockOperation
		EXPECT_THROW(
			auto op = documents::operations::indexes::SetIndexesLockOperation(example_index.name, documents::indexes::IndexLockMode::UNSET),
			std::invalid_argument);
		
		ASSERT_TRUE(does_index_exist_by_get_index_op(example_index.name));
		{
			auto op = documents::operations::indexes::GetIndexOperation(example_index.name);
			auto cmd = op.get_command({});
			test_suite_executor->get().execute(*cmd);
			auto&& res = cmd->get_result();
			EXPECT_EQ(res->lock_mode, documents::indexes::IndexLockMode::UNLOCK);
		}
		{
			auto op = documents::operations::indexes::SetIndexesLockOperation(example_index.name, documents::indexes::IndexLockMode::LOCKED_ERROR);
			auto cmd = op.get_command({});
			test_suite_executor->get().execute(*cmd);
			auto&& res = cmd->get_result();
		}
		{
			auto op = documents::operations::indexes::GetIndexOperation(example_index.name);
			auto cmd = op.get_command({});
			test_suite_executor->get().execute(*cmd);
			auto&& res = cmd->get_result();
			ASSERT_EQ(res->lock_mode, documents::indexes::IndexLockMode::LOCKED_ERROR);
		}
	}

	TEST_F(IndexOperationsTestOld, CanSetIndexPriority)
	{
		//should NOT use IndexPriority::UNSET for SetIndexesPriorityOperation
		EXPECT_THROW(
			auto op = documents::operations::indexes::SetIndexesPriorityOperation(example_index.name, documents::indexes::IndexPriority::UNSET),
			std::invalid_argument);

		ASSERT_TRUE(does_index_exist_by_get_index_op(example_index.name));
		{
			auto op = documents::operations::indexes::GetIndexOperation(example_index.name);
			auto cmd = op.get_command({});
			test_suite_executor->get().execute(*cmd);
			auto&& res = cmd->get_result();
			EXPECT_EQ(res->priority, documents::indexes::IndexPriority::NORMAL);
		}
		{
			auto op = documents::operations::indexes::SetIndexesPriorityOperation(example_index.name, documents::indexes::IndexPriority::HIGH);
			auto cmd = op.get_command({});
			test_suite_executor->get().execute(*cmd);
		}
		{
			auto op = documents::operations::indexes::GetIndexOperation(example_index.name);
			auto cmd = op.get_command({});
			test_suite_executor->get().execute(*cmd);
			auto&& res = cmd->get_result();
			ASSERT_EQ(res->priority, documents::indexes::IndexPriority::HIGH);
		}
	}

	TEST_F(IndexOperationsTestOld, CanListErrors)
	{
		{
			auto invalid_user = example_user;
			invalid_user.age = 0;
			nlohmann::json invalid_user_json = invalid_user;
			invalid_user_json["@metadata"]["@collection"] = "Users";
			documents::commands::PutDocumentCommand cmd(invalid_user.id, {}, invalid_user_json);
			test_suite_executor->get().execute(cmd);
		}

		auto invalid_index = example_index;
		invalid_index.name = example_index.name + "1";
		invalid_index.maps = { R"(
				from user in docs.Users 
				select new 
				{
					a = 5 / user.age
				})" };
		{
			auto op = documents::operations::indexes::PutIndexesOperation({ invalid_index });
			auto cmd = op.get_command({});
			test_suite_executor->get().execute(*cmd);
		}
		//waiting for indexing TODO something better
		std::this_thread::sleep_for(std::chrono::duration<int>(3));

		ASSERT_TRUE(does_index_exist_by_get_index_op(invalid_index.name));
		ASSERT_TRUE(does_index_exist_by_get_index_op(example_index.name));

		auto op1 = documents::operations::indexes::GetIndexErrorsOperation();
		auto cmd1 = op1.get_command({});
		test_suite_executor->get().execute(*cmd1);
		auto&& res1 = cmd1->get_result();
		const auto ASSERT_ERROR_PER_INDEX = [&](const std::string& index_name, std::size_t expected_size)
		{
			ASSERT_EQ(expected_size, 
				std::find_if(res1->cbegin(), res1->cend(),
				[&](const documents::indexes::IndexErrors& err)
				{return err.name == index_name; })->errors.size());
		};

		ASSERT_ERROR_PER_INDEX(example_index.name, 0);
		ASSERT_ERROR_PER_INDEX(invalid_index.name, 1);

		auto op2 = documents::operations::indexes::GetIndexErrorsOperation({invalid_index.name});
		auto cmd2 = op2.get_command({});
		test_suite_executor->get().execute(*cmd2);
		auto&& res2 = cmd2->get_result();
		ASSERT_EQ(res2->size(), 1);
		ASSERT_ERROR_PER_INDEX(invalid_index.name, 1);
	}

	TEST_F(IndexOperationsTestOld, CanGetIndexStatistics)
	{
		ASSERT_TRUE(does_index_exist_by_get_index_op(example_index.name));
		{
			auto op = documents::operations::indexes::GetIndexesStatisticsOperation();
			auto cmd = op.get_command({});
			test_suite_executor->get().execute(*cmd);
			auto&& res = cmd->get_result();

			ASSERT_EQ(res->size(), 1);
			ASSERT_EQ((*res)[0].name, example_index.name);
			ASSERT_EQ((*res)[0].entries_count, 1);
		}
		{
			infrastructure::entities::User user = { "Users/2","Johnnie","Walker","GB",0,150 };
			nlohmann::json user_json = user;
			user_json["@metadata"]["@collection"] = "Users";
			documents::commands::PutDocumentCommand cmd(user.id, {}, user_json);
			test_suite_executor->get().execute(cmd);
		}
		{
			auto op = documents::operations::indexes::GetIndexesStatisticsOperation();
			auto cmd = op.get_command({});
			test_suite_executor->get().execute(*cmd);
			auto&& res = cmd->get_result();

			ASSERT_EQ(res->size(), 1);
			ASSERT_EQ((*res)[0].name, example_index.name);
			ASSERT_EQ((*res)[0].entries_count, 1);
			ASSERT_TRUE((*res)[0].is_stale);
		}
	}
}
