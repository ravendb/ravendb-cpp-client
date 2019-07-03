#include "pch.h"
#include "RavenTestDriver.h"
#include "DocumentSession.h"
#include "AdvancedSessionOperations.h"
#include "raven_test_definitions.h"
#include "User.h"
#include "Person.h"

namespace ravendb::client::tests::client::documents
{
	class BasicDocumentsTest : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<ravendb::client::documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}
	};

	TEST_F(BasicDocumentsTest, CanChangeDocumentCollectionWithDeleteAndSave)
	{
		auto store = get_document_store(TEST_NAME);

		auto document_id = std::string{ "users/1" };

		{
			auto session = store->open_session();
			auto user = std::make_shared<infrastructure::entities::User>();
			user->name = "Alexander";
			session.store(user, document_id);
			session.save_changes();
		}
		{
			auto session = store->open_session();
			session.delete_document(document_id);
			session.save_changes();
		}
		{
			auto session = store->open_session();
			auto user = session.load<infrastructure::entities::User>(document_id);
			ASSERT_FALSE(user);
		}
		{
			auto session = store->open_session();
			auto person = std::make_shared<infrastructure::entities::Person>();
			person->name = "Alexander";
			session.store(person, document_id);
			session.save_changes();
		}
	}

	TEST_F(BasicDocumentsTest, GetTests)
	{
		auto store = get_document_store(TEST_NAME);

		nlohmann::json dummy = infrastructure::entities::User();
		dummy.erase("id");
		{
			auto session = store->open_session();
			auto user1 = std::make_shared<infrastructure::entities::User>();
			user1->name = "Alexander";

			auto user2 = std::make_shared<infrastructure::entities::User>();
			user2->name = "Alexey";

			session.store(user1, "users/1");
			session.store(user2, "users/2");

			session.save_changes();
		}

		auto request_executor = store->get_request_executor();

		{
			auto get_document_command = ravendb::client::documents::commands::GetDocumentsCommand(
				std::vector<std::string>{ "users/1", "users/2" }, {}, false);

			request_executor->execute(get_document_command);

			auto docs = get_document_command.get_result();
			ASSERT_EQ(2, docs->results.size());

			const auto& doc1 = docs->results.at(0);
			const auto& doc2 = docs->results.at(1);

			ASSERT_FALSE(doc1.is_null());

			std::vector<std::string> doc1_properties{};
			for (const auto&[key, value] : static_cast<const nlohmann::json::object_t&>(doc1))
			{
				doc1_properties.emplace_back(key);
			}

			ASSERT_TRUE(std::find(doc1_properties.cbegin(), doc1_properties.cend(), "@metadata") !=
				doc1_properties.end());

			ASSERT_EQ(dummy.size() + 1, doc1_properties.size());// +1 for @metadata

			ASSERT_FALSE(doc2.is_null());

			std::vector<std::string> doc2_properties{};
			for (const auto&[key, value] : static_cast<const nlohmann::json::object_t&>(doc2))
			{
				doc2_properties.emplace_back(key);
			}

			ASSERT_TRUE(std::find(doc2_properties.cbegin(), doc2_properties.cend(), "@metadata") !=
				doc2_properties.end());

			ASSERT_EQ(dummy.size() + 1, doc2_properties.size());// +1 for @metadata

			auto user1 = doc1.get<infrastructure::entities::User>();
			auto user2 = doc2.get<infrastructure::entities::User>();

			ASSERT_EQ("Alexander", user1.name);
			ASSERT_EQ("Alexey", user2.name);

		}
		auto get_document_command = ravendb::client::documents::commands::GetDocumentsCommand(
			std::vector<std::string>{ "users/1", "users/2" }, {}, true);

		request_executor->execute(get_document_command);

		auto docs = get_document_command.get_result();

		ASSERT_EQ(2, docs->results.size());

		const auto& doc1 = docs->results.at(0);
		const auto& doc2 = docs->results.at(1);

		ASSERT_FALSE(doc1.is_null());

		std::vector<std::string> doc1_properties{};
		for (const auto&[key, value] : static_cast<const nlohmann::json::object_t&>(doc1))
		{
			doc1_properties.emplace_back(key);
		}

		ASSERT_TRUE(std::find(doc1_properties.cbegin(), doc1_properties.cend(), "@metadata") !=
			doc1_properties.end());

		ASSERT_EQ(1, doc1_properties.size());//1 for @metadata

		ASSERT_FALSE(doc2.is_null());

		std::vector<std::string> doc2_properties{};
		for (const auto&[key, value] : static_cast<const nlohmann::json::object_t&>(doc2))
		{
			doc2_properties.emplace_back(key);
		}

		ASSERT_TRUE(std::find(doc2_properties.cbegin(), doc2_properties.cend(), "@metadata") !=
			doc2_properties.end());

		ASSERT_EQ(1, doc2_properties.size());//1 for @metadata
	}
}

