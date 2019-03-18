#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "AdvancedSessionOperations.h"
#include "RequestExecutor.h"
#include "User.h"
#include "PutDocumentCommand.h"

namespace ravendb::client::tests::client::documents::commands
{
	class PutDocumentCommandTest : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<ravendb::client::documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}
	};

	TEST_F(PutDocumentCommandTest, CanPutDocumentUsingCommand)
	{
		auto store = get_document_store(TEST_NAME);

		auto user = std::make_shared<infrastructure::entities::User>();
		user->name = "Alexander";
		user->age = 38;

		nlohmann::json json = *user;

		auto command = ravendb::client::documents::commands::PutDocumentCommand("users/1", {}, json);
		store->get_request_executor()->execute(command);

		auto res = command.get_result();

		ASSERT_EQ("users/1", res->id);
		ASSERT_FALSE(res->change_vector.empty());

		{
			auto session = store->open_session();
			auto loaded_user = session.load<infrastructure::entities::User>("users/1");
			ASSERT_EQ("Alexander", loaded_user->name);
		}
	}
}
