#include "pch.h"
#include "re_definitions.h"
#include "User.h"
#include "GetDocumentsCommand.h"
#include "PutDocumentCommand.h"
#include "DeleteDocumentCommand.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"

namespace ravendb::client::tests::old_tests
{
	class SecuredRequestExecutorTests : public driver::RavenTestDriver
	{
	protected:
		inline static std::shared_ptr<documents::DocumentStore> document_store{};
		inline static bool is_faulted;
		inline static std::string why_faulted;
		static const infrastructure::entities::User example_user;

		void SetUp() override//create sample document
		{
			try
			{
				document_store = get_secured_document_store(TEST_NAME);
			}
			catch(const std::runtime_error& e)
			{
				is_faulted = true;
				why_faulted = std::string(e.what());
			}

			if(is_faulted)
				FAIL() << why_faulted;

			const nlohmann::json j = example_user;
			documents::commands::PutDocumentCommand cmd(example_user.id, {}, j);
			document_store->get_request_executor()->execute(cmd);
		}

		void TearDown() override //delete sample document
		{
			if(!is_faulted)
			{
				documents::commands::DeleteDocumentCommand cmd(example_user.id);
				document_store->get_request_executor()->execute(cmd);
				document_store.reset();
			}
		}

		static bool does_document_exist(const std::string& doc_id)
		{
			documents::commands::GetDocumentsCommand cmd(doc_id, {}, true);
			document_store->get_request_executor()->execute(cmd);
			auto res = cmd.get_result();

			return res && !res->results.empty() && !res->results[0].is_null();
		}
	};

	const infrastructure::entities::User SecuredRequestExecutorTests::example_user{ "users/1", "Alexander", "Timoshenko", "Israel", 0, 38 };

	TEST_F(SecuredRequestExecutorTests, CanGetDocument)
	{
		ASSERT_TRUE(does_document_exist(example_user.id));

		documents::commands::GetDocumentsCommand cmd(example_user.id, {}, false);
		document_store->get_request_executor()->execute(cmd);
		auto res = cmd.get_result();

		const auto check_user = res->results[0].get<infrastructure::entities::User>();
		ASSERT_EQ(example_user, check_user);
	}

	TEST_F(SecuredRequestExecutorTests, CanDeleteDocument)
	{
		ASSERT_TRUE(does_document_exist(example_user.id));

		documents::commands::DeleteDocumentCommand cmd(example_user.id);
		document_store->get_request_executor()->execute(cmd);

		ASSERT_FALSE(does_document_exist(example_user.id));
	}
}
