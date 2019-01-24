#include "pch.h"
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


namespace ravendb::client::tests
{
	class IndexOperationsTest : public ::testing::Test
	{
	protected:
		inline static std::shared_ptr<RequestExecutorScope> test_suite_executor{};

		static const User example_user;
		static IndexDefinition example_index;

		static void SetUpTestCase()
		{
			test_suite_executor = GET_REQUEST_EXECUTOR();
			set_example_index();
		}
		static void TearDownTestCase()
		{
			test_suite_executor.reset();
		}

		void SetUp() override//create sample document and sample index
		{
			nlohmann::json j = example_user;
			documents::commands::PutDocumentCommand cmd(example_user.id, {}, j);
			test_suite_executor->get()->execute(cmd);

			auto op = documents::operations::indexes::PutIndexesOperation({ example_index });
			test_suite_executor->get()->execute(op.get_command({}));

			//waiting for indexing TODO something better
			std::this_thread::sleep_for(std::chrono::duration<int>(3));
		}

		void TearDown() override //delete all documents and all indexes
		{
			auto op1 = documents::operations::indexes::GetIndexNamesOperation(0, 100);
			auto&& res2 = test_suite_executor->get()->execute(op1.get_command({}));
			for (auto& index_name : res2)
			{
				auto op = documents::operations::indexes::DeleteIndexOperation(index_name);
				test_suite_executor->get()->execute(op.get_command({}));
			}
			
			auto cmd = documents::commands::GetDocumentsCommand(0, 100);
			auto&& res1 = test_suite_executor->get()->execute(cmd);
			for(auto& result : res1.results)
			{
				User u = result;
				documents::commands::DeleteDocumentCommand cmd2(u.id);
				test_suite_executor->get()->execute(cmd2);
			}
		}

		bool does_index_exist_by_get_index_op(const std::string& index_name)
		{
			auto op = documents::operations::indexes::GetIndexOperation(index_name);
			try
			{
				auto&& res = test_suite_executor->get()->execute(op.get_command({}));
			}catch (RavenError&)
			{
				return false;
			}
			return true;
		}

		bool does_index_exist_by_get_index_names_op(const std::string& index_name)
		{
			auto op = documents::operations::indexes::GetIndexNamesOperation(0, 10);
			auto&& res = test_suite_executor->get()->execute(op.get_command({}));
			return std::find(res.cbegin(), res.cend(), index_name) != res.cend();
		}

		static void set_example_index()
		{
			example_index.name = "UsersIndex";
			example_index.maps = { R"(
				from user in docs.Users 
				select new 
				{
					Name = user.FirstName + " " + user.LastName
				})"};
		}
	};
	const User IndexOperationsTest::example_user{ "Users/1", "Alexander", "Timoshenko", "Israel", 0, 38 };
	IndexDefinition IndexOperationsTest::example_index{};
	
	TEST_F(IndexOperationsTest, CanGetIndex)
	{
		ASSERT_TRUE(does_index_exist_by_get_index_op(example_index.name));

		auto op = documents::operations::indexes::GetIndexOperation(example_index.name);
		auto&& res = test_suite_executor->get()->execute(op.get_command({}));

		IndexDefinition check_index =
		{
			example_index.name,
			documents::indexes::IndexPriority::NORMAL,
			documents::indexes::IndexLockMode::UNLOCK,{},
			example_index.maps,{},{},{},
			documents::indexes::IndexType::MAP,{}
		};
		ASSERT_EQ(res, check_index);
	}

	TEST_F(IndexOperationsTest, CanDeleteIndex)
	{
		ASSERT_TRUE(does_index_exist_by_get_index_names_op(example_index.name));

		auto op = documents::operations::indexes::DeleteIndexOperation(example_index.name);
		auto&& res = test_suite_executor->get()->execute(op.get_command({}));

		ASSERT_FALSE(does_index_exist_by_get_index_names_op(example_index.name));
	}

	TEST_F(IndexOperationsTest, CanDisableAndEnableIndex)
	{
		ASSERT_TRUE(does_index_exist_by_get_index_op(example_index.name));
		{
			auto op = documents::operations::indexes::GetIndexingStatusOperation();
			auto&& res = test_suite_executor->get()->execute(op.get_command({}));
			EXPECT_EQ(res.indexes[0].status, IndexRunningStatus::RUNNING);
		}
		{
			auto op = documents::operations::indexes::DisableIndexOperation(example_index.name);
			test_suite_executor->get()->execute(op.get_command({}));
		}
		{
			auto op = documents::operations::indexes::GetIndexingStatusOperation();
			auto&& res = test_suite_executor->get()->execute(op.get_command({}));
			ASSERT_EQ(res.indexes[0].status, IndexRunningStatus::DISABLED);
		}

		{
			auto op = documents::operations::indexes::EnableIndexOperation(example_index.name);
			test_suite_executor->get()->execute(op.get_command({}));
		}
		{
			auto op = documents::operations::indexes::GetIndexingStatusOperation();
			auto&& res = test_suite_executor->get()->execute(op.get_command({}));
			ASSERT_EQ(res.indexes[0].status, IndexRunningStatus::RUNNING);
		}
	}

	TEST_F(IndexOperationsTest, CanGetIndexes)
	{
		ASSERT_TRUE(does_index_exist_by_get_index_op(example_index.name));

		auto op = documents::operations::indexes::GetIndexesOperation(0,10);
		auto&& res = test_suite_executor->get()->execute(op.get_command({}));

		ASSERT_EQ(res.size(), 1);
		ASSERT_EQ(res[0].name, example_index.name);
	}

	TEST_F(IndexOperationsTest, CanGetIndexesStats)
	{
		ASSERT_TRUE(does_index_exist_by_get_index_op(example_index.name));

		auto op = documents::operations::indexes::GetIndexesStatisticsOperation();
		auto&& res = test_suite_executor->get()->execute(op.get_command({}));

		ASSERT_EQ(res.size(), 1);
		ASSERT_EQ(res[0].name, example_index.name);
	}

	TEST_F(IndexOperationsTest, CanGetTerms)
	{
		ASSERT_TRUE(does_index_exist_by_get_index_op(example_index.name));

		auto op = documents::operations::indexes::GetTermsOperation(example_index.name,"Name");
		auto&& res = test_suite_executor->get()->execute(op.get_command({}));

		ASSERT_EQ(res.size(), 1);
		std::string expected_str = example_user.first_name + ' ' + example_user.last_name;
		std::transform(expected_str.cbegin(), expected_str.cend(), expected_str.begin(),
			[](std::string::value_type c) {return std::tolower(c); });
		ASSERT_EQ(res[0], expected_str);
	}

	TEST_F(IndexOperationsTest, HasIndexChanged)
	{
		ASSERT_TRUE(does_index_exist_by_get_index_op(example_index.name));
		{
			auto op = documents::operations::indexes::IndexHasChangedOperation(example_index);
			auto&& res = test_suite_executor->get()->execute(op.get_command({}));

			ASSERT_FALSE(res);
		}
		auto changed_index = example_index;
		changed_index.maps = { R"(
				from user in docs.Users 
				select new 
				{
					FullName = user.FirstName + ' ' + user.LastName + " from " + user.Address
				})" };
		{
			auto op = documents::operations::indexes::IndexHasChangedOperation(changed_index);
			auto&& res = test_suite_executor->get()->execute(op.get_command({}));

			ASSERT_TRUE(res);
		}
	}

	TEST_F(IndexOperationsTest, CanStopStartIndexing)
	{
		ASSERT_TRUE(does_index_exist_by_get_index_op(example_index.name));
		{
			auto op = documents::operations::indexes::GetIndexingStatusOperation();
			auto&& res = test_suite_executor->get()->execute(op.get_command({}));
			EXPECT_EQ(res.status, IndexRunningStatus::RUNNING);
		}
		{
			auto op = documents::operations::indexes::StopIndexingOperation();
			test_suite_executor->get()->execute(op.get_command({}));
		}
		{
			auto op = documents::operations::indexes::GetIndexingStatusOperation();
			auto&& res = test_suite_executor->get()->execute(op.get_command({}));
			ASSERT_EQ(res.status, IndexRunningStatus::PAUSED);
		}

		{
			auto op = documents::operations::indexes::StartIndexingOperation();
			test_suite_executor->get()->execute(op.get_command({}));
		}
		{
			auto op = documents::operations::indexes::GetIndexingStatusOperation();
			auto&& res = test_suite_executor->get()->execute(op.get_command({}));
			ASSERT_EQ(res.status, IndexRunningStatus::RUNNING);
		}
	}

	TEST_F(IndexOperationsTest, CanStopStartIndex)
	{
		ASSERT_TRUE(does_index_exist_by_get_index_op(example_index.name));
		{
			auto op = documents::operations::indexes::GetIndexingStatusOperation();
			auto&& res = test_suite_executor->get()->execute(op.get_command({}));
			EXPECT_EQ(res.status, IndexRunningStatus::RUNNING);
			EXPECT_EQ(res.indexes[0].status, IndexRunningStatus::RUNNING);
		}
		{
			auto op = documents::operations::indexes::StopIndexOperation(example_index.name);
			test_suite_executor->get()->execute(op.get_command({}));
		}
		{
			auto op = documents::operations::indexes::GetIndexingStatusOperation();
			auto&& res = test_suite_executor->get()->execute(op.get_command({}));
			ASSERT_EQ(res.status, IndexRunningStatus::RUNNING);
			ASSERT_EQ(res.indexes[0].status, IndexRunningStatus::PAUSED);
		}

		{
			auto op = documents::operations::indexes::StartIndexOperation(example_index.name);
			test_suite_executor->get()->execute(op.get_command({}));
		}
		{
			auto op = documents::operations::indexes::GetIndexingStatusOperation();
			auto&& res = test_suite_executor->get()->execute(op.get_command({}));
			ASSERT_EQ(res.status, IndexRunningStatus::RUNNING);
			ASSERT_EQ(res.indexes[0].status, IndexRunningStatus::RUNNING);
		}
	}

	TEST_F(IndexOperationsTest, CanSetIndexLockMode)
	{
		//should NOT use IndexLockMode::UNSET for SetIndexesLockOperation
		EXPECT_THROW(
			auto op = documents::operations::indexes::SetIndexesLockOperation(example_index.name, IndexLockMode::UNSET),
			std::invalid_argument);
		
		ASSERT_TRUE(does_index_exist_by_get_index_op(example_index.name));
		{
			auto op = documents::operations::indexes::GetIndexOperation(example_index.name);
			auto&& res = test_suite_executor->get()->execute(op.get_command({}));
			EXPECT_EQ(res.lock_mode, documents::indexes::IndexLockMode::UNLOCK);
		}
		{
			auto op = documents::operations::indexes::SetIndexesLockOperation(example_index.name, IndexLockMode::LOCKED_ERROR);
			auto&& res = test_suite_executor->get()->execute(op.get_command({}));
		}
		{
			auto op = documents::operations::indexes::GetIndexOperation(example_index.name);
			auto&& res = test_suite_executor->get()->execute(op.get_command({}));
			ASSERT_EQ(res.lock_mode, documents::indexes::IndexLockMode::LOCKED_ERROR);
		}
	}

	TEST_F(IndexOperationsTest, CanSetIndexPriority)
	{
		//should NOT use IndexPriority::UNSET for SetIndexesPriorityOperation
		EXPECT_THROW(
			auto op = documents::operations::indexes::SetIndexesPriorityOperation(example_index.name, IndexPriority::UNSET),
			std::invalid_argument);

		ASSERT_TRUE(does_index_exist_by_get_index_op(example_index.name));
		{
			auto op = documents::operations::indexes::GetIndexOperation(example_index.name);
			auto&& res = test_suite_executor->get()->execute(op.get_command({}));
			EXPECT_EQ(res.priority, documents::indexes::IndexPriority::NORMAL);
		}
		{
			auto op = documents::operations::indexes::SetIndexesPriorityOperation(example_index.name, IndexPriority::HIGH);
			auto&& res = test_suite_executor->get()->execute(op.get_command({}));
		}
		{
			auto op = documents::operations::indexes::GetIndexOperation(example_index.name);
			auto&& res = test_suite_executor->get()->execute(op.get_command({}));
			ASSERT_EQ(res.priority, documents::indexes::IndexPriority::HIGH);
		}
	}

	TEST_F(IndexOperationsTest, CanListErrors)
	{
		auto invalid_user = example_user;
		invalid_user.age = 0;
		documents::commands::PutDocumentCommand cmd(invalid_user.id, {}, nlohmann::json(invalid_user));
		test_suite_executor->get()->execute(cmd);

		auto invalid_index = example_index;
		invalid_index.name = example_index.name + "1";
		invalid_index.maps = { R"(
				from user in docs.Users 
				select new 
				{
					a = 5 / user.Age
				})" };
		{
			auto op = documents::operations::indexes::PutIndexesOperation({ invalid_index });
			test_suite_executor->get()->execute(op.get_command({}));
		}
		//waiting for indexing TODO something better
		std::this_thread::sleep_for(std::chrono::duration<int>(3));

		ASSERT_TRUE(does_index_exist_by_get_index_op(invalid_index.name));
		ASSERT_TRUE(does_index_exist_by_get_index_op(example_index.name));

		auto op1 = documents::operations::indexes::GetIndexErrorsOperation();
		auto res = test_suite_executor->get()->execute(op1.get_command({}));
		const auto ASSERT_ERROR_PER_INDEX = [&](const std::string& index_name, std::size_t expected_size)
		{
			ASSERT_EQ(expected_size, 
				std::find_if(res.cbegin(), res.cend(),
				[&](const IndexErrors& err)
				{return err.name == index_name; })->errors.size());
		};

		ASSERT_ERROR_PER_INDEX(example_index.name, 0);
		ASSERT_ERROR_PER_INDEX(invalid_index.name, 1);

		auto op2 = documents::operations::indexes::GetIndexErrorsOperation({invalid_index.name});
		res = test_suite_executor->get()->execute(op2.get_command({}));
		ASSERT_EQ(res.size(), 1);
		ASSERT_ERROR_PER_INDEX(invalid_index.name, 1);
	}

	TEST_F(IndexOperationsTest, CanGetIndexStatistics)
	{
		ASSERT_TRUE(does_index_exist_by_get_index_op(example_index.name));
		{
			auto op = documents::operations::indexes::GetIndexesStatisticsOperation();
			auto&& res = test_suite_executor->get()->execute(op.get_command({}));

			ASSERT_EQ(res.size(), 1);
			ASSERT_EQ(res[0].name, example_index.name);
			ASSERT_EQ(res[0].entries_count, 1);
		}
		{
			User user = { "Users/2","Johnnie","Walker","GB",0,150 };
			documents::commands::PutDocumentCommand cmd(user.id, {}, nlohmann::json(user));
			test_suite_executor->get()->execute(cmd);
		}
		{
			auto op = documents::operations::indexes::GetIndexesStatisticsOperation();
			auto&& res = test_suite_executor->get()->execute(op.get_command({}));

			ASSERT_EQ(res.size(), 1);
			ASSERT_EQ(res[0].name, example_index.name);
			ASSERT_EQ(res[0].entries_count, 1);
			ASSERT_TRUE(res[0].is_stale);
		}
	}
}
