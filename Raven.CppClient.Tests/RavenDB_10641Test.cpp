#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "EntityIdHelperUtil.h"
#include "MetadataAsDictionary.h"
#include "AdvancedSessionOperations.h"

namespace ravendb::client::tests::issues
{
	struct Document
	{
		std::string id;
	};

	void from_json(const nlohmann::json& j, Document& )
	{}

	void to_json(nlohmann::json& j, const Document& )
	{}

	class RavenDB_10641Test : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<client::documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}

		static void SetUpTestCase()
		{
			REGISTER_ID_PROPERTY_FOR(Document, id);
		}
	};

	TEST_F(RavenDB_10641Test, CanEditObjectsInMetadata)
	{
		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();

			auto doc = std::make_shared<Document>();
			session.store(doc, "items/first");

			auto items = json::MetadataAsDictionary();
			items.insert_or_assign("lang", "en");
			session.advanced().get_metadata_for(doc)->insert_or_assign("Items", std::move(items));

			session.save_changes();
		}
		{
			auto session = store->open_session();

			auto doc = session.load<Document>("items/first");
			auto& items = session.advanced().get_metadata_for(doc)->get_as_dict("Items");

			ASSERT_EQ("en", items.get_as<std::string>("lang"));

			items.insert_or_assign("lang", "sv");

			session.save_changes();
		}
		{
			auto session = store->open_session();

			auto v = session.load<Document>("items/first");
			auto& metadata = *session.advanced().get_metadata_for(v);
			metadata.insert_or_assign("test", "123");

			session.save_changes();
		}
		{
			auto session = store->open_session();

			auto v = session.load<Document>("items/first");
			auto& metadata = *session.advanced().get_metadata_for(v);

			ASSERT_EQ("sv", metadata.get_as_dict("Items").get_as<std::string>("lang"));

			ASSERT_EQ("123", metadata.get_as<std::string>("test"));
		}
	}
}
