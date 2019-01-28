#include "pch.h"
#define __USE_FIDDLER__
#include "ds_definitions.h"
#include "DocumentSession.h"
#include "SessionOptions.h"
#include "DocumentStore.h"
#include "User.h"
#include "Family.h"

namespace ravendb::client::tests
{
	class CrudTests : public ::testing::Test
	{
	protected:
		inline static std::shared_ptr<DocumentStoreScope> test_suite_store{};

		static void SetUpTestCase()
		{
			test_suite_store = GET_DOCUMENT_STORE();
		}
		static void TearDownTestCase()
		{
			test_suite_store.reset();
		}
	};

	TEST_F(CrudTests, EntitiesAreSavedUsingLowerCase)
	{
		{
			auto session = test_suite_store->get().open_session();
			auto user1 = std::make_shared<User>();
			user1->last_name = "user1";
			session.store(user1, std::string("users/1"));
			session.save_changes();
		}

		auto documents_command = documents::commands::GetDocumentsCommand("users/1", {}, false);
		test_suite_store->get().get_request_executor()->execute(documents_command);
		auto result = documents_command.get_result();

		auto user_json = result.results.at(0);
		ASSERT_TRUE(user_json.find("LastName") != user_json.end());

		//TODO complete using session.advanced().raw_query
	}

	TEST_F(CrudTests, CrudOperations)
	{
		auto session = test_suite_store->get().open_session();
		auto user1 = std::make_shared<User>();
		user1->last_name = "user1";
		session.store(user1, std::string("users/1"));
		
		auto user2 = std::make_shared<User>();
		user2->first_name = "user2";
		user1->age = 1;
		session.store(user2, std::string("users/2"));

		auto user3 = std::make_shared<User>();
		user3->first_name = "user3";
		user3->age = 1;
		session.store(user3, std::string("users/3"));

		auto user4 = std::make_shared<User>();
		user4->first_name = "user4";
		session.store(user4, std::string("users/4"));

		session.delete_document(user2);
		user3->age = 3;
		session.save_changes();

		auto temp_user = session.load<User>("users/2");
		ASSERT_FALSE(temp_user);

		temp_user = session.load<User>("users/3");
		ASSERT_EQ(3, temp_user->age);

		user1 = session.load<User>("users/1");
		user4 = session.load<User>("users/4");

		session.delete_document(user4);
		user1->age = 10;
		session.save_changes();

		temp_user = session.load<User>("users/4");
		ASSERT_FALSE(temp_user);

		temp_user = session.load<User>("users/1");
		ASSERT_EQ(10, temp_user->age);		
	}

	TEST_F(CrudTests, CrudOperationsWithWhatChanged)
	{
		auto session = test_suite_store->get().open_session();
		auto user1 = std::make_shared<User>();
		user1->last_name = "user1";
		session.store(user1, std::string("users/1"));

		auto user2 = std::make_shared<User>();
		user2->first_name = "user2";
		user1->age = 1;
		session.store(user2, std::string("users/2"));

		auto user3 = std::make_shared<User>();
		user3->first_name = "user3";
		user3->age = 1;
		session.store(user3, std::string("users/3"));

		auto user4 = std::make_shared<User>();
		user4->first_name = "user4";
		session.store(user4, std::string("users/4"));

		session.delete_document(user2);
		user3->age = 3;

		ASSERT_EQ(4, session.advanced().what_changed().size());
		session.save_changes();

		auto temp_user = session.load<User>("users/2");
		ASSERT_FALSE(temp_user);

		temp_user = session.load<User>("users/3");
		ASSERT_EQ(3, temp_user->age);

		user1 = session.load<User>("users/1");
		user4 = session.load<User>("users/4");

		session.delete_document(user4);
		user1->age = 10;
		ASSERT_EQ(2, session.advanced().what_changed().size());

		session.save_changes();

		temp_user = session.load<User>("users/4");
		ASSERT_FALSE(temp_user);

		temp_user = session.load<User>("users/1");
		ASSERT_EQ(10, temp_user->age);
	}

	TEST_F(CrudTests, CrudOperationsWithArrayInObject)
	{
		auto session = test_suite_store->get().open_session();

		auto family = std::make_shared<Family>();
		family->names = { "Hibernating Rhinos", "RavenDB" };
		session.store(family, std::string("family/1"));
		session.save_changes();

		auto new_family = session.load<Family>("family/1");
		new_family->names = { "Toli", "Mitzi", "Boki" };
		ASSERT_EQ(1, session.advanced().what_changed().size());
		session.save_changes();
	}
	TEST_F(CrudTests, CrudOperationsWithArrayInObject2)
	{
		auto session = test_suite_store->get().open_session();

		auto family = std::make_shared<Family>();
		family->names = { "Hibernating Rhinos", "RavenDB" };
		session.store(family, std::string("family/1"));
		session.save_changes();

		auto new_family = session.load<Family>("family/1");
		new_family->names = { "Hibernating Rhinos", "RavenDB" };
		ASSERT_EQ(0, session.advanced().what_changed().size());

		new_family->names = { "RavenDB","Hibernating Rhinos" };
		ASSERT_EQ(1, session.advanced().what_changed().size());
		session.save_changes();
	}

	TEST_F(CrudTests, CrudOperationsWithArrayInObject3)
	{
		auto session = test_suite_store->get().open_session();

		auto family = std::make_shared<Family>();
		family->names = { "Hibernating Rhinos", "RavenDB" };
		session.store(family, std::string("family/1"));
		session.save_changes();

		auto new_family = session.load<Family>("family/1");
		new_family->names = { "RavenDB" };
		ASSERT_EQ(1, session.advanced().what_changed().size());
		session.save_changes();
	}

	TEST_F(CrudTests, CrudOperationsWithArrayInObject4)
	{
		auto session = test_suite_store->get().open_session();

		auto family = std::make_shared<Family>();
		family->names = { "Hibernating Rhinos", "RavenDB" };
		session.store(family, std::string("family/1"));
		session.save_changes();

		auto new_family = session.load<Family>("family/1");
		new_family->names = { "RavenDB", "Hibernating Rhinos", "Toli", "Mitzi", "Boki" };
		ASSERT_EQ(1, session.advanced().what_changed().size());
		session.save_changes();
	}

	TEST_F(CrudTests, CrudOperationsWithNull)
	{
		auto session = test_suite_store->get().open_session();

		auto user = std::make_shared<User>();
		user->first_name = {};

		session.store(user, std::string("users/1"));
		session.save_changes();

		auto user2 = session.load<User>("users/1");
		ASSERT_TRUE(session.advanced().what_changed().empty());

		user2->age = 3;
		ASSERT_EQ(1, session.advanced().what_changed().size());
	}

	TEST_F(CrudTests, CrudOperationsWithArrayOfObjects)
	{
		auto session = test_suite_store->get().open_session();

		auto member1 = std::make_shared<Member>();
		member1->name = "Hibernating Rhinos";
		member1->age = 8;

		auto member2 = std::make_shared<Member>();
		member2->name = "RavenDB";
		member2->age = 4;

		auto family = std::make_shared<FamilyMembers>();
		family->members = { member1, member2 };

		session.store(family, std::string("family/1"));
		session.save_changes();

		member1 = std::make_shared<Member>();
		member1->name = "RavenDB";
		member1->age = 4;

		member2 = std::make_shared<Member>();
		member2->name = "Hibernating Rhinos";
		member2->age = 8;

		auto new_family = session.load<FamilyMembers>("family/1");
		new_family->members = { member1, member2 };

		auto changes = session.advanced().what_changed();
		ASSERT_EQ(1, changes.size());

		ASSERT_EQ(4, changes.at("family/1").size());

		ASSERT_EQ(std::string("Age"), changes.at("family/1").at(0).field_name);
		ASSERT_EQ(documents::session::DocumentsChanges::ChangeType::FIELD_CHANGED, changes.at("family/1").at(0).change);
		ASSERT_EQ(8, changes.at("family/1").at(0).field_old_value.get<int>());
		ASSERT_EQ(4, changes.at("family/1").at(0).field_new_value.get<int>());
		
		ASSERT_EQ(std::string("Name"), changes.at("family/1").at(1).field_name);
		ASSERT_EQ(documents::session::DocumentsChanges::ChangeType::FIELD_CHANGED, changes.at("family/1").at(1).change);
		ASSERT_EQ(std::string("Hibernating Rhinos"), changes.at("family/1").at(1).field_old_value.get<std::string>());
		ASSERT_EQ(std::string("RavenDB"), changes.at("family/1").at(1).field_new_value.get<std::string>());

		ASSERT_EQ(std::string("Age"), changes.at("family/1").at(2).field_name);
		ASSERT_EQ(documents::session::DocumentsChanges::ChangeType::FIELD_CHANGED, changes.at("family/1").at(2).change);
		ASSERT_EQ(4, changes.at("family/1").at(2).field_old_value.get<int>());
		ASSERT_EQ(8, changes.at("family/1").at(2).field_new_value.get<int>());

		ASSERT_EQ(std::string("Name"), changes.at("family/1").at(3).field_name);
		ASSERT_EQ(documents::session::DocumentsChanges::ChangeType::FIELD_CHANGED, changes.at("family/1").at(3).change);
		ASSERT_EQ(std::string("RavenDB"), changes.at("family/1").at(3).field_old_value.get<std::string>());
		ASSERT_EQ(std::string("Hibernating Rhinos"), changes.at("family/1").at(3).field_new_value.get<std::string>());

		member1 = std::make_shared<Member>();
		member1->name = "Toli";
		member1->age = 5;

		member2 = std::make_shared<Member>();
		member2->name = "Boki";
		member2->age = 15;

		new_family->members = { member1, member2 };

		changes = session.advanced().what_changed();
		ASSERT_EQ(1, changes.size());

		ASSERT_EQ(4, changes.at("family/1").size());

		ASSERT_EQ(std::string("Age"), changes.at("family/1").at(0).field_name);
		ASSERT_EQ(documents::session::DocumentsChanges::ChangeType::FIELD_CHANGED, changes.at("family/1").at(0).change);
		ASSERT_EQ(8, changes.at("family/1").at(0).field_old_value.get<int>());
		ASSERT_EQ(5, changes.at("family/1").at(0).field_new_value.get<int>());

		ASSERT_EQ(std::string("Name"), changes.at("family/1").at(1).field_name);
		ASSERT_EQ(documents::session::DocumentsChanges::ChangeType::FIELD_CHANGED, changes.at("family/1").at(1).change);
		ASSERT_EQ(std::string("Hibernating Rhinos"), changes.at("family/1").at(1).field_old_value.get<std::string>());
		ASSERT_EQ(std::string("Toli"), changes.at("family/1").at(1).field_new_value.get<std::string>());

		ASSERT_EQ(std::string("Age"), changes.at("family/1").at(2).field_name);
		ASSERT_EQ(documents::session::DocumentsChanges::ChangeType::FIELD_CHANGED, changes.at("family/1").at(2).change);
		ASSERT_EQ(4, changes.at("family/1").at(2).field_old_value.get<int>());
		ASSERT_EQ(15, changes.at("family/1").at(2).field_new_value.get<int>());

		ASSERT_EQ(std::string("Name"), changes.at("family/1").at(3).field_name);
		ASSERT_EQ(documents::session::DocumentsChanges::ChangeType::FIELD_CHANGED, changes.at("family/1").at(3).change);
		ASSERT_EQ(std::string("RavenDB"), changes.at("family/1").at(3).field_old_value.get<std::string>());
		ASSERT_EQ(std::string("Boki"), changes.at("family/1").at(3).field_new_value.get<std::string>());
	}
}
