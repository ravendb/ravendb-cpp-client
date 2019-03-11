#include "pch.h"
//#define __USE_FIDDLER__
#include "TestSuiteBase.h"
#include "DocumentSession.h"
#include "User.h"
#include "EntityIdHelperUtil.h"
#include "AdvancedSessionOperations.h"
#include "GetDocumentsCommand.h"

CREATE_ENTITY_ID_HELPER_FOR(ravendb::client::tests::infrastructure::entities::User, id);

namespace ravendb::client::tests::client::documents
{
	class DocumentIdPropertyTest : public infrastructure::TestSuiteBase
	{
	protected:
		static void SetUpTestCase()
		{
			test_suite_store = definitions::GET_DOCUMENT_STORE();

			register_entity_id_helper<infrastructure::entities::User>();
		}
	};

	TEST_F(DocumentIdPropertyTest, CanSetIdPropertyFromStore)
	{
		{
			auto session = test_suite_store->get()->open_session();

			auto user = std::make_shared<infrastructure::entities::User>();
			session.store(user, "users/1");
			ASSERT_EQ("users/1", user->id);

			session.save_changes();
		}
		{
			auto session = test_suite_store->get()->open_session();

			auto user = session.load<infrastructure::entities::User>("users/1");
			ASSERT_EQ("users/1", user->id);
		}
	}

	TEST_F(DocumentIdPropertyTest, CanSetIdPropertyFromHilo)
	{
		std::string user_id{};
		{
			auto session = test_suite_store->get()->open_session();

			auto user = std::make_shared<infrastructure::entities::User>();
			session.store(user);
			user_id = user->id;

			ASSERT_FALSE(user_id.empty());

			session.save_changes();
		}
		{
			auto session = test_suite_store->get()->open_session();

			auto user = session.load<infrastructure::entities::User>(user_id);
			ASSERT_EQ(user_id, user->id);
		}
	}

	TEST_F(DocumentIdPropertyTest, CanGetIdFromProperty)
	{
		std::string user_id = "custom_user_id/1";
		{
			auto session = test_suite_store->get()->open_session();

			auto user = std::make_shared<infrastructure::entities::User>();
			user->id = user_id;
			session.store(user);
			ASSERT_EQ(user_id, *session.advanced().get_document_id(user));

			session.save_changes();
		}
		{
			auto session = test_suite_store->get()->open_session();

			auto get_doc_command = ravendb::client::documents::commands::GetDocumentsCommand(user_id, {}, true);
			auto re = session.advanced().get_request_executor();
			re->execute(get_doc_command);

			ASSERT_EQ(user_id, get_doc_command.get_result().results[0].at("@metadata").at("@id"));
		}
	}
}