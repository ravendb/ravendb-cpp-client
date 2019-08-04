#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "AdvancedSessionOperations.h"
#include "MetadataAsDictionary.h"
#include "EntityIdHelperUtil.h"
#include "User.h"

namespace ravendb::client::tests::issues
{
	class RavenDB_10566Test : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<client::documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}

		static void SetUpTestCase()
		{
			REGISTER_ID_PROPERTY_FOR(infrastructure::entities::User, id);
		}
	};

	TEST_F(RavenDB_10566Test, ShouldBeAvailable)
	{
		auto store = get_document_store(TEST_NAME);

		auto name = std::shared_ptr<std::string>();

		store->add_after_save_changes_listener(std::function<void(const documents::DocumentStore*, const documents::session::AfterSaveChangesEventArgs&)>(
			[&](const documents::DocumentStore*, const documents::session::AfterSaveChangesEventArgs& event)
		{
			name = std::make_shared<std::string>(
				event.get_document_metadata()->get_as<std::string>("Name"));
		}));

		{
			auto session = store->open_session();

			auto user = std::make_shared<infrastructure::entities::User>();
			user->name = "Alexander";

			session.store(user, "users/alexander");
			auto& metadata = *session.advanced().get_metadata_for(user);

			metadata.insert_or_assign("Name", "FooBar");

			session.save_changes();
		}
		ASSERT_EQ("FooBar", *name);
	}
}
