#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "User.h"
#include "EntityIdHelperUtil.h"
#include "AdvancedSessionOperations.h"
#include "GetDocumentsCommand.h"

namespace ravendb::client::tests::client::documents
{
	class DocumentIdPropertyTest : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<ravendb::client::documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}

		static void SetUpTestCase()
		{
			REGISTER_ID_PROPERTY_FOR(ravendb::client::tests::infrastructure::entities::User, id);
		}
	};

	TEST_F(DocumentIdPropertyTest, CanSetIdPropertyFromStore)
	{
		auto store = get_document_store(TEST_NAME);

		{
			auto session = store->open_session();

			auto user = std::make_shared<infrastructure::entities::User>();
			session.store(user, "users/1");
			ASSERT_EQ("users/1", user->id);

			session.save_changes();
		}
		{
			auto session = store->open_session();

			auto user = session.load<infrastructure::entities::User>("users/1");
			ASSERT_EQ("users/1", user->id);
		}
	}

	TEST_F(DocumentIdPropertyTest, CanSetIdPropertyFromHilo)
	{
		auto store = get_document_store(TEST_NAME);

		std::string user_id{};

		{
			auto session = store->open_session();

			auto user = std::make_shared<infrastructure::entities::User>();
			session.store(user);
			user_id = user->id;

			ASSERT_FALSE(user_id.empty());

			session.save_changes();
		}
		{
			auto session = store->open_session();

			auto user = session.load<infrastructure::entities::User>(user_id);
			ASSERT_EQ(user_id, user->id);
		}
	}

	TEST_F(DocumentIdPropertyTest, CanGetIdFromProperty)
	{
		auto store = get_document_store(TEST_NAME);
		
		std::string user_id = "custom_user_id/1";
		
		{
			auto session = store->open_session();

			auto user = std::make_shared<infrastructure::entities::User>();
			user->id = user_id;
			session.store(user);
			ASSERT_EQ(user_id, *session.advanced().get_document_id(user));

			session.save_changes();
		}
		{
			auto session = store->open_session();

			auto get_doc_command = ravendb::client::documents::commands::GetDocumentsCommand(user_id, {}, true);
			auto re = session.advanced().get_request_executor();
			re->execute(get_doc_command);

			ASSERT_EQ(user_id, get_doc_command.get_result()->results[0].at("@metadata").at("@id"));
		}
	}
}