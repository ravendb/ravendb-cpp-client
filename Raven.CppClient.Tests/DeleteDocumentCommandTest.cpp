#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "AdvancedSessionOperations.h"
#include "RequestExecutor.h"
#include "User.h"
#include "EntityIdHelperUtil.h"
#include "DeleteDocumentCommand.h"

CREATE_ENTITY_ID_HELPER_FOR(ravendb::client::tests::infrastructure::entities::User, id);

namespace ravendb::client::tests::client::documents::commands
{
	class DeleteDocumentCommandTest : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<ravendb::client::documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}

		static void SetUpTestCase()
		{
			register_entity_id_helper<infrastructure::entities::User>();
		}
	};

	TEST_F(DeleteDocumentCommandTest, CanDeleteDocument)
	{
		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();
			auto user = std::make_shared<infrastructure::entities::User>();
			user->name = "Alexander";
			session.store(user, "users/1");
			session.save_changes();
		}

		auto command = ravendb::client::documents::commands::DeleteDocumentCommand("users/1");
		store->get_request_executor()->execute(command);

		{
			auto session = store->open_session();
			auto loaded_user = session.load<infrastructure::entities::User>("users/1");
			ASSERT_FALSE(loaded_user);
		}
	}

	TEST_F(DeleteDocumentCommandTest, CanDeleteDocumentByEtag)
	{
		auto store = get_document_store(TEST_NAME);

		std::optional<std::string> change_vector{};
		{
			auto session = store->open_session();
			auto user = std::make_shared<infrastructure::entities::User>();
			user->name = "Alexander";
			session.store(user, "users/1");
			session.save_changes();

			change_vector = session.advanced().get_change_vector_for(user);
			ASSERT_TRUE(change_vector.has_value());
		}
		{
			auto session = store->open_session();
			auto loaded_user = session.load<infrastructure::entities::User>("users/1");
			loaded_user->age = 38;
			session.save_changes();			
		}

		auto command = ravendb::client::documents::commands::DeleteDocumentCommand("users/1", change_vector);
		try
		{
			store->get_request_executor()->execute(command);
		}
		catch (std::exception& e)
		{
			//TODO assert that this is ConcurrencyException
			SUCCEED();
			return;
		}
		FAIL();
	}
}
