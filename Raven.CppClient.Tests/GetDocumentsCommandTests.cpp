#include "pch.h"
//#define __USE_FIDDLER__
#include "re_definitions.h"
#include "GetDocumentsCommand.h"
#include "CreateSampleDataOperation.h"

using ravendb::client::documents::commands::GetDocumentsCommand;

namespace ravendb::client::tests::old_tests
{
	class GetDocumentsCommandTests : public ::testing::Test
	{
	protected:
		static std::shared_ptr<definitions::RequestExecutorScope> test_suite_executor;

		static void SetUpTestCase()
		{
			test_suite_executor = definitions::GET_REQUEST_EXECUTOR();
			auto op = infrastructure::CreateSampleDataOperation();
			auto cmd = op.get_command({});
			test_suite_executor->get().execute(*cmd);
		}
		static void TearDownTestCase()
		{
			test_suite_executor.reset();
		}

	};

	std::shared_ptr<definitions::RequestExecutorScope> GetDocumentsCommandTests::test_suite_executor{};

	TEST_F(GetDocumentsCommandTests, CanGetPagedSetOfDocuments)
	{
		GetDocumentsCommand cmd1(0, INT32_MAX);
		test_suite_executor->get().execute(cmd1);
		auto all_docs = cmd1.get_result();
		int32_t num_of_docs = (int32_t)all_docs->results.size();
		int32_t half_num_of_docs = num_of_docs / 2;

		GetDocumentsCommand cmd2(0, half_num_of_docs);
		test_suite_executor->get().execute(cmd2);
		auto half_docs = cmd2.get_result();
		ASSERT_EQ(half_docs->results.size(), half_num_of_docs);

		int32_t rand_num = rand() % half_num_of_docs;
		GetDocumentsCommand cmd3(half_num_of_docs, rand_num);
		test_suite_executor->get().execute(cmd3);
		auto some_docs = cmd3.get_result();
		ASSERT_EQ(some_docs->results.size(), rand_num);
	}

	TEST_F(GetDocumentsCommandTests, CanGetMultipleDocumentsUsingPost)
	{
		const int NUM_OF_ORDERS = 500;
		std::vector<std::string> orders_list{};
		for(int i=1; i <= NUM_OF_ORDERS; ++i)
		{
			orders_list.push_back(std::string("orders/") + std::to_string(i) + "-A");
		}
		GetDocumentsCommand cmd(orders_list, {}, true);
		test_suite_executor->get().execute(cmd);
		auto orders = cmd.get_result()->results;
		ASSERT_EQ(orders.size(), NUM_OF_ORDERS);
	}

	TEST_F(GetDocumentsCommandTests, CanGetMultipleDocumentsWithMissingDocuments)
	{
		const int NUM_OF_ORDERS = 50;
		std::vector<std::string> orders_list{};
		for (int i = 1; i <= NUM_OF_ORDERS; ++i)
		{
			if (i % 2 != 0)
			{
				orders_list.push_back(std::string("orders/") + std::to_string(-i) + "-A");
			}
			else
			{
				orders_list.push_back(std::string("orders/") + std::to_string(i) + "-A");
			}
		}
		GetDocumentsCommand cmd(orders_list, {}, true);
		test_suite_executor->get().execute(cmd);
		auto orders = cmd.get_result()->results;
		for (int i = 1; i <= NUM_OF_ORDERS; ++i)
		{
			if (i % 2 != 0)
			{
				ASSERT_TRUE(orders[i-1].is_null());
			}
			else
			{
				auto num_of_metadata_vals = orders[i-1].at("@metadata").size();
				ASSERT_GT(num_of_metadata_vals, 0);
			}
		}
	}

	TEST_F(GetDocumentsCommandTests, CanGetDocumentsByIdsWithIncludes)
	{
		const int NUM_OF_ORDERS = 10;
		const int START_ORDER = 1;
		std::vector<std::string> orders_list;
		for (int i = START_ORDER; i < START_ORDER+NUM_OF_ORDERS; ++i) 
		{
			orders_list.push_back(std::string("orders/") + std::to_string(i) + "-A");
		}
		GetDocumentsCommand cmd(orders_list, {"Company"}, true);
		test_suite_executor->get().execute(cmd);
		auto res = cmd.get_result();
		ASSERT_EQ(res->results.size(), NUM_OF_ORDERS);
		ASSERT_EQ(res->includes.size(), NUM_OF_ORDERS-1);//there is a duplicate
	}

	TEST_F(GetDocumentsCommandTests, CanGetDocumentsByStartsWith)
	{
		const std::string starts_with = "or";
		const std::string starts_after = "orders/100-A";
		const std::string matches = "*10?-A";
		const std::string exclude = "*5*";

		GetDocumentsCommand cmd(starts_with, starts_after, matches, exclude, 0, 100, true);
		test_suite_executor->get().execute(cmd);
		auto res = cmd.get_result();
		ASSERT_EQ(res->results.size(), 8);//101-A to 109-A without 105-A
		ASSERT_EQ(res->includes.size(), 0);
	}
}
