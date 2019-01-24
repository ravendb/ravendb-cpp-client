#include "pch.h"
#include "re_definitions.h"
#include "CreateSampleDataOperation.h"
#include "QueryCommand.h"
#include "QueryOperationOptions.h"
#include "DeleteByQueryOperation.h"
#include "DocumentStore.h"

namespace
{
	class FakeStore : public ravendb::client::documents::DocumentStore
	{};
}

namespace ravendb::client::tests
{
	class BasicQueryTests : public ::testing::Test
	{
	protected:
		inline static std::shared_ptr<RequestExecutorScope> test_suite_executor{};

		static void SetUpTestCase()
		{
			test_suite_executor = GET_REQUEST_EXECUTOR();
			auto op = infrastructure::CreateSampleDataOperation();
			test_suite_executor->get()->execute(op.get_command({}));
		}
		static void TearDownTestCase()
		{
			test_suite_executor.reset();
		}
	};

	TEST_F(BasicQueryTests, CanExecuteParametersQuery)
	{
		const std::string query = R"(
			from Employees
			where StartsWith(FirstName,$prefix)
		)";
		const std::string prefix = "An";

		IndexQuery index_query(query);
		index_query.query_parameters.emplace("prefix", prefix);
		index_query.wait_for_non_stale_results = true;
		index_query.wait_for_non_stale_results_timeout = std::chrono::seconds(10);

		auto cmd = documents::commands::QueryCommand({}, index_query, false, false);
		auto&& res = test_suite_executor->get()->execute(cmd);

		//essential assertions
		ASSERT_EQ(res.total_results, 2);
		ASSERT_EQ(res.skipped_results, 0);
		ASSERT_EQ(res.total_results, res.results.size());
		for(const auto& result : res.results)
		{
			ASSERT_EQ(prefix, result.at("FirstName").get<std::string>().substr(0, 2));
		}
		//misc assertions
		ASSERT_TRUE(res.includes.empty());
		ASSERT_GE(res.duration_in_ms, 0);
		ASSERT_FALSE(res.index_name.empty());
	}

	TEST_F(BasicQueryTests, CanExecuteQueryWithIncludes)
	{
		const std::string query = R"(
			from Orders
			where search(Company,"*/?-*")
			include Company 
		)";
		IndexQuery index_query(query);
		index_query.wait_for_non_stale_results = true;
		index_query.wait_for_non_stale_results_timeout = std::chrono::seconds(10);

		auto cmd = documents::commands::QueryCommand({}, index_query, false, false);
		auto&& res = test_suite_executor->get()->execute(cmd);

		//results assertions
		ASSERT_EQ(res.total_results, 86);
		ASSERT_EQ(res.skipped_results, 0);
		ASSERT_EQ(res.total_results, res.results.size());
		//includes assertions
		ASSERT_EQ(res.includes.size(), 9);
		ASSERT_EQ(res.included_paths.size(), 1);
		ASSERT_EQ("Company", res.included_paths[0]);
	}

	TEST_F(BasicQueryTests, CandDeleteByQuery)
	{
		const std::string categories_query = "from Categories";
		IndexQuery categories_index_query(categories_query);
		categories_index_query.wait_for_non_stale_results = true;
		categories_index_query.wait_for_non_stale_results_timeout = std::chrono::seconds(1);
		
		auto cmd1 = documents::commands::QueryCommand({}, categories_index_query, true, false);
		auto&& res1 = test_suite_executor->get()->execute(cmd1);
		//Categories count
		const auto total_categories = res1.total_results;
		
		const std::string custom_query = R"(
			from Categories
			where StartsWith(Name,$prefix)
		)";
		const std::string prefix = "M";

		IndexQuery custom_index_query(custom_query);
		custom_index_query.query_parameters.emplace("prefix", prefix);
		custom_index_query.wait_for_non_stale_results = true;
		custom_index_query.wait_for_non_stale_results_timeout = std::chrono::seconds(1);

		auto cmd2 = documents::commands::QueryCommand({}, custom_index_query, true, false);
		auto&& res2 = test_suite_executor->get()->execute(cmd2);
		//Custom query results count
		const auto custom_query_tot_res = res2.total_results;
		ASSERT_GE(total_categories, custom_query_tot_res);

		documents::queries::QueryOperationOptions query_op_options({}, false, std::chrono::seconds(1), false);
		auto op3 = documents::operations::DeleteByQueryOperation(custom_index_query, query_op_options);
		FakeStore store;
		HttpCache cache;
		auto&& res3 = test_suite_executor->get()->execute(op3.get_command(store, {}, cache));
		ASSERT_GT(res3.operation_id, 0);

		std::this_thread::sleep_for(std::chrono::seconds(1));//TODO something better

		auto&& res4 = test_suite_executor->get()->execute(cmd1);
		const auto categories_left = res4.total_results;
		//Rest of employees = All employees - Custom query results
		ASSERT_EQ(total_categories - custom_query_tot_res, categories_left);
	}
}
