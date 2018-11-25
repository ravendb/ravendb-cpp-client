#include "pch.h"
#include "definitions.h"

#include "GetDocumentsCommand.h"
#include "CreateSampleDataOperation.h"

using ravendb::client::documents::commands::GetDocumentsCommand;

namespace ravendb::client::tests
{
	class GetDocumentsCommandTests : public ::testing::Test
	{
	protected:
		inline static std::unique_ptr<RequestExecutorScope> test_suite_executor{};

		static void SetUpTestCase()
		{
			test_suite_executor = GET_REQUEST_EXECUTOR();
			infrastructure::CreateSampleDataCommand cmd;
			test_suite_executor->get()->execute(cmd);
		}
	};

	TEST_F(GetDocumentsCommandTests, CanGetPagedSetOfDocuments)
	{
		GetDocumentsCommand cmd1(0, INT32_MAX);
		auto all_docs = test_suite_executor->get()->execute(cmd1);
		int32_t num_of_docs = all_docs.results.size();
		int32_t half_num_of_docs = num_of_docs / 2;

		GetDocumentsCommand cmd2(0, half_num_of_docs);
		auto half_docs = test_suite_executor->get()->execute(cmd2);
		ASSERT_EQ(half_docs.results.size(), half_num_of_docs);

		int32_t rand_num = rand() % half_num_of_docs;
		GetDocumentsCommand cmd3(half_num_of_docs, rand_num);
		auto some_docs = test_suite_executor->get()->execute(cmd3);
		ASSERT_EQ(some_docs.results.size(), rand_num);
	}

	TEST_F(GetDocumentsCommandTests, CanGetMultipleDocumentsUsingPost)
	{
		const int NUM_OF_ORDERS = 500;
		std::vector<std::string> orders_list;
		for(int i=1; i <= NUM_OF_ORDERS; ++i)
		{
			orders_list.push_back(std::string("orders/") + std::to_string(i) + "-A");
		}
		GetDocumentsCommand cmd(orders_list, {}, true);
		auto orders = test_suite_executor->get()->execute(cmd).results;
		ASSERT_EQ(orders.size(), NUM_OF_ORDERS);
	}

	TEST_F(GetDocumentsCommandTests, CanGetMultipleDocumentsWithMissingDocuments)
	{
		const int NUM_OF_ORDERS = 50;
		std::vector<std::string> orders_list;
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
		auto orders = test_suite_executor->get()->execute(cmd).results;
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

	TEST_F(GetDocumentsCommandTests, CanGetDocumentsWithIncludes)
	{

	}
}
