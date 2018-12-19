#include "pch.h"
#include "definitions.h"
#include "CreateSampleDataOperation.h"
#include "QueryCommand.h"

namespace ravendb::client::tests
{
	class BasicQueryCommandTests : public ::testing::Test
	{
	protected:
		inline static std::unique_ptr<RequestExecutorScope> test_suite_executor{};

		static void SetUpTestCase()
		{
			test_suite_executor = GET_REQUEST_EXECUTOR();
			auto op = infrastructure::CreateSampleDataOperation();
			test_suite_executor->get()->execute(op.get_command({}));
		}
	};

	TEST_F(BasicQueryCommandTests, CanExecuteParametersQuery)
	{
		const std::string query = R"(
			from Employees
			where StartsWith(FirstName,$prefix)
		)";
		const std::string prefix = "An";

		IndexQuery indexQuery(query);
		indexQuery.query_parameters.emplace("prefix", prefix);
		indexQuery.wait_for_non_stale_results = true;
		indexQuery.wait_for_non_stale_results_timeout = std::chrono::seconds(10);

		auto cmd = documents::commands::QueryCommand({}, indexQuery, false, false);
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

	TEST_F(BasicQueryCommandTests, CanExecuteQueryWithIncludes)
	{
		const std::string query = R"(
			from Orders
			where search(Company,"*/?-*")
			include Company 
		)";

		IndexQuery indexQuery(query);
		indexQuery.wait_for_non_stale_results = true;
		indexQuery.wait_for_non_stale_results_timeout = std::chrono::seconds(10);

		auto cmd = documents::commands::QueryCommand({}, indexQuery, false, false);
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
}