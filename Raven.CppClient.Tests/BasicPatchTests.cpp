#include "pch.h"
//#define __USE_FIDDLER__
#include "re_definitions.h"
#include "User.h"
#include "PatchOperation.h"
#include "PutDocumentCommand.h"
#include "GetDocumentsCommand.h"
#include "PatchByQueryOperation.h"
#include "DeleteDocumentCommand.h"
#include "GetOperationStateOperation.h"
#include "DocumentStore.h"
#include "ds_definitions.h"

namespace ravendb::client::tests::old_tests
{
	class BasicPatchTests : public ::testing::Test
	{
	protected:
		inline static std::shared_ptr<definitions::RequestExecutorScope> test_suite_executor{};

		static void SetUpTestCase()
		{
			test_suite_executor = definitions::GET_REQUEST_EXECUTOR();
		}
		static void TearDownTestCase()
		{
			test_suite_executor.reset();
		}

		void TearDown() override
		{
			auto  get_docs_cmd = documents::commands::GetDocumentsCommand({}, {}, {}, {},0,100,true);
			auto results = test_suite_executor->get().execute(get_docs_cmd);
			for (const auto& res : results.results)
			{
				auto del_doc_cmd = documents::commands::DeleteDocumentCommand(res["@metadata"]["@id"].get<std::string>());
				test_suite_executor->get().execute(del_doc_cmd);
			}
		}
	};

	TEST_F(BasicPatchTests, CanPatchSingleDocument)
	{
		infrastructure::entities::User user{"users/1","Alexander","Timoshenko","Israel"};
		nlohmann::json user_json = user;
		user_json["@metadata"]["@collection"] = "Users";

		auto put_doc_cmd = documents::commands::PutDocumentCommand(user.id, {}, user_json);
		auto&& res1 = test_suite_executor->get().execute(put_doc_cmd);
		ASSERT_EQ(res1.id, user.id);

		auto op = documents::operations::PatchOperation(user.id, {},
			documents::operations::PatchRequest(R"(this.LastName = "The Great")"));

		HttpCache cache;
		auto&& res2 = test_suite_executor->get().execute(op.get_command(
			documents::DocumentStore::create(), {}, cache));

		auto check_user = user;
		check_user.last_name = "The Great";
		auto modified_user = nlohmann::json(res2.modified_document).get<infrastructure::entities::User>();

		ASSERT_EQ(res2.status, documents::operations::PatchStatus::PATCHED);
		ASSERT_EQ(check_user, modified_user);

		auto get_doc_cmd = documents::commands::GetDocumentsCommand(user.id, {}, false);
		auto&& res3= test_suite_executor->get().execute(get_doc_cmd);

		infrastructure::entities::User ret_user = res3.results[0].get<infrastructure::entities::User>();
		ASSERT_EQ(check_user, ret_user);
	}

	TEST_F(BasicPatchTests, CanPatchManyDocuments)
	{
		constexpr size_t NUM_OF_USERS = 5;
		std::array<infrastructure::entities::User, NUM_OF_USERS> users{};

		for (size_t i = 0; i < users.size(); ++i)
		{
			users[i].name = "Alexander";
			users[i].last_name = std::to_string(i+1);
			users[i].id = "users/" + std::to_string(i + 1);

			nlohmann::json user_json = users[i];
			user_json["@metadata"]["@collection"] = "Users";

			auto put_doc_cmd = documents::commands::PutDocumentCommand(users[i].id, {}, user_json);
			auto res = test_suite_executor->get().execute(put_doc_cmd);
			ASSERT_EQ(res.id, users[i].id);
		}

		std::string update_query = R"(
			from Users as user
			update 
			{
				user.LastName += $PlusGreatness
			})";

		auto update_index_query = documents::queries::IndexQuery(update_query);
		update_index_query.query_parameters = { {"PlusGreatness", " Great"} };
		auto op = documents::operations::PatchByQueryOperation(update_index_query);

		{
			HttpCache cache;
			auto&& res = test_suite_executor->get().execute(op.get_command(
				documents::DocumentStore::create(), {}, cache));
			EXPECT_GT(res.operation_id, 0);
		}
		//wait for completion
		std::this_thread::sleep_for(std::chrono::seconds(3));//TODO something better

		auto get_doc_cmd = documents::commands::GetDocumentsCommand(0,100);
		auto&& res = test_suite_executor->get().execute(get_doc_cmd);
		for (size_t i = 0; i < users.size(); ++i)
		{
			infrastructure::entities::User u = res.results[users.size() - i - 1];
			ASSERT_EQ(u.last_name, std::to_string(i + 1) + " Great");
		}	
	}

	TEST_F(BasicPatchTests, ThrowsOnInvalidScript)
	{
		infrastructure::entities::User user{ "users/1","Alexander","Timoshenko","Israel" };
		nlohmann::json user_json = user;
		user_json["@metadata"]["@collection"] = "Users";

		auto put_doc_cmd = documents::commands::PutDocumentCommand(user.id, {},user_json);
		auto&& res1 = test_suite_executor->get().execute(put_doc_cmd);
		ASSERT_EQ(res1.id, user.id);

		std::string update_query = R"(
			from Users
			update 
			{
				throw "Error"
			})";
		auto op2 = documents::operations::PatchByQueryOperation(update_query);
		HttpCache cache;
		auto&& res2 = test_suite_executor->get().execute(op2.get_command(
			documents::DocumentStore::create(), {}, cache));

		std::this_thread::sleep_for(std::chrono::seconds(3));//TODO use TBD waitForCompletion()

		auto op3 = documents::operations::GetOperationStateOperation(res2.operation_id);
		auto&& res3 = test_suite_executor->get().execute(op3.get_command({}));

		ASSERT_EQ(res3["Status"].get<std::string>(), "Faulted");
		ASSERT_EQ(res3["Result"]["Type"].get<std::string>(), "Raven.Client.Exceptions.Documents.Patching.JavaScriptException");
	}
}
