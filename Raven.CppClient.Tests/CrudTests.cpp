#include "pch.h"
//#define __USE_FIDDLER__
#include "ds_definitions.h"
#include "DocumentSession.h"
#include "SessionOptions.h"
#include "DocumentStore.h"
#include "User.h"
#include "Family.h"
#include "NullableUser.h"
#include "Arr.h"
#include "DeleteDocumentCommand.h"
#include "Poc.h"

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

		void TearDown() override
		{
			auto  get_docs_cmd = documents::commands::GetDocumentsCommand({}, {}, {}, {}, 0, 100, true);
			auto results = test_suite_store->get().get_request_executor()->execute(get_docs_cmd);
			for (const auto& res : results.results)
			{
				auto del_doc_cmd = documents::commands::DeleteDocumentCommand(res["@metadata"]["@id"].get<std::string>());
				test_suite_store->get().get_request_executor()->execute(del_doc_cmd);
			}
		}

		documents::session::DocumentInfo::ToJsonConverter arr2_to_json = [](std::shared_ptr<void> arr_param)->nlohmann::json
		{
			auto arr = std::static_pointer_cast<Arr2>(arr_param);
			nlohmann::json json_array = nlohmann::json::array();
			for(const auto& a : arr->arr1)
			{
				nlohmann::json temp = nlohmann::json::object();
				temp["str"] = nlohmann::json(a.str);
				json_array.push_back(std::move(temp));
			}
			nlohmann::json json;
			json["arr1"] = std::move(json_array);
			return json;
		};

		documents::session::DocumentInfo::FromJsonConverter arr2_from_json = [](const nlohmann::json& json)->std::shared_ptr<void>
		{
			auto arr2 = std::make_shared<Arr2>();
			const auto& json_array = static_cast<const nlohmann::json::array_t&>(json["arr1"]);
			arr2->arr1.reserve(json_array.size());
			for(const auto& str_arr : json_array)
			{
				Arr1 arr1{};
				arr1.str.reserve(str_arr.at("str").size());
				for (const auto& str : str_arr.at("str"))
				{
					arr1.str.push_back(str.get<std::string>());
				}
				arr2->arr1.push_back(std::move(arr1));
			}
			return std::static_pointer_cast<void>(arr2);
		};
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

		auto user = std::make_shared<NullableUser>();

		session.store(user, std::string("n_users/1"));
		session.save_changes();

		auto user2 = session.load<NullableUser>("n_users/1");
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

		session.store(family, std::string("family_members/1"));
		session.save_changes();

		member1 = std::make_shared<Member>();
		member1->name = "RavenDB";
		member1->age = 4;

		member2 = std::make_shared<Member>();
		member2->name = "Hibernating Rhinos";
		member2->age = 8;

		auto new_family = session.load<FamilyMembers>("family_members/1");
		new_family->members = { member1, member2 };

		auto changes = session.advanced().what_changed();
		ASSERT_EQ(1, changes.size());

		ASSERT_EQ(4, changes.at("family_members/1").size());

		ASSERT_EQ(std::string("Age"), changes.at("family_members/1").at(0).field_name);
		ASSERT_EQ(documents::session::DocumentsChanges::ChangeType::FIELD_CHANGED, changes.at("family_members/1").at(0).change);
		ASSERT_EQ(8, changes.at("family_members/1").at(0).field_old_value.get<int>());
		ASSERT_EQ(4, changes.at("family_members/1").at(0).field_new_value.get<int>());
		
		ASSERT_EQ(std::string("Name"), changes.at("family_members/1").at(1).field_name);
		ASSERT_EQ(documents::session::DocumentsChanges::ChangeType::FIELD_CHANGED, changes.at("family_members/1").at(1).change);
		ASSERT_EQ(std::string("Hibernating Rhinos"), changes.at("family_members/1").at(1).field_old_value.get<std::string>());
		ASSERT_EQ(std::string("RavenDB"), changes.at("family_members/1").at(1).field_new_value.get<std::string>());

		ASSERT_EQ(std::string("Age"), changes.at("family_members/1").at(2).field_name);
		ASSERT_EQ(documents::session::DocumentsChanges::ChangeType::FIELD_CHANGED, changes.at("family_members/1").at(2).change);
		ASSERT_EQ(4, changes.at("family_members/1").at(2).field_old_value.get<int>());
		ASSERT_EQ(8, changes.at("family_members/1").at(2).field_new_value.get<int>());

		ASSERT_EQ(std::string("Name"), changes.at("family_members/1").at(3).field_name);
		ASSERT_EQ(documents::session::DocumentsChanges::ChangeType::FIELD_CHANGED, changes.at("family_members/1").at(3).change);
		ASSERT_EQ(std::string("RavenDB"), changes.at("family_members/1").at(3).field_old_value.get<std::string>());
		ASSERT_EQ(std::string("Hibernating Rhinos"), changes.at("family_members/1").at(3).field_new_value.get<std::string>());

		member1 = std::make_shared<Member>();
		member1->name = "Toli";
		member1->age = 5;

		member2 = std::make_shared<Member>();
		member2->name = "Boki";
		member2->age = 15;

		new_family->members = { member1, member2 };

		changes = session.advanced().what_changed();
		ASSERT_EQ(1, changes.size());

		ASSERT_EQ(4, changes.at("family_members/1").size());

		ASSERT_EQ(std::string("Age"), changes.at("family_members/1").at(0).field_name);
		ASSERT_EQ(documents::session::DocumentsChanges::ChangeType::FIELD_CHANGED, changes.at("family_members/1").at(0).change);
		ASSERT_EQ(8, changes.at("family_members/1").at(0).field_old_value.get<int>());
		ASSERT_EQ(5, changes.at("family_members/1").at(0).field_new_value.get<int>());

		ASSERT_EQ(std::string("Name"), changes.at("family_members/1").at(1).field_name);
		ASSERT_EQ(documents::session::DocumentsChanges::ChangeType::FIELD_CHANGED, changes.at("family_members/1").at(1).change);
		ASSERT_EQ(std::string("Hibernating Rhinos"), changes.at("family_members/1").at(1).field_old_value.get<std::string>());
		ASSERT_EQ(std::string("Toli"), changes.at("family_members/1").at(1).field_new_value.get<std::string>());

		ASSERT_EQ(std::string("Age"), changes.at("family_members/1").at(2).field_name);
		ASSERT_EQ(documents::session::DocumentsChanges::ChangeType::FIELD_CHANGED, changes.at("family_members/1").at(2).change);
		ASSERT_EQ(4, changes.at("family_members/1").at(2).field_old_value.get<int>());
		ASSERT_EQ(15, changes.at("family_members/1").at(2).field_new_value.get<int>());

		ASSERT_EQ(std::string("Name"), changes.at("family_members/1").at(3).field_name);
		ASSERT_EQ(documents::session::DocumentsChanges::ChangeType::FIELD_CHANGED, changes.at("family_members/1").at(3).change);
		ASSERT_EQ(std::string("RavenDB"), changes.at("family_members/1").at(3).field_old_value.get<std::string>());
		ASSERT_EQ(std::string("Boki"), changes.at("family_members/1").at(3).field_new_value.get<std::string>());
	}

	TEST_F(CrudTests, CrudOperationsWithArrayOfArrays)
	{
		{
			auto session = test_suite_store->get().open_session();

			Arr1 a1 = { {"a", "b"} };
			Arr1 a2 = { {"c", "d"} };
			auto arr = std::make_shared<Arr2>();
			arr->arr1 = { a1, a2 };

			session.store(arr, std::string("arr/1"), arr2_to_json);
			session.save_changes();

			auto new_arr = session.load<Arr2>("arr/1");

			a1.str = { "d", "c" };
			a2.str = { "a", "b" };
			new_arr->arr1 = { a1, a2 };

			auto what_changed = session.advanced().what_changed();
			ASSERT_EQ(1, what_changed.size());

			auto change = what_changed.at("arr/1");
			ASSERT_EQ(4, change.size());

			ASSERT_EQ(std::string("a"), change.at(0).field_old_value.get<std::string>());
			ASSERT_EQ(std::string("d"), change.at(0).field_new_value.get<std::string>());


			ASSERT_EQ(std::string("b"), change.at(1).field_old_value.get<std::string>());
			ASSERT_EQ(std::string("c"), change.at(1).field_new_value.get<std::string>());

			ASSERT_EQ(std::string("c"), change.at(2).field_old_value.get<std::string>());
			ASSERT_EQ(std::string("a"), change.at(2).field_new_value.get<std::string>());

			ASSERT_EQ(std::string("d"), change.at(3).field_old_value.get<std::string>());
			ASSERT_EQ(std::string("b"), change.at(3).field_new_value.get<std::string>());

			session.save_changes();
		}
		{
			auto session = test_suite_store->get().open_session();

			auto new_arr = session.load<Arr2>("arr/1", arr2_from_json, arr2_to_json);

			Arr1 a1 = { {"q", "w"} };
			Arr1 a2 = { {"a", "b"} };
			new_arr->arr1 = { a1, a2 };

			auto what_changed = session.advanced().what_changed();
			ASSERT_EQ(1, what_changed.size());

			auto change = what_changed.at("arr/1");
			ASSERT_EQ(2, change.size());

			ASSERT_EQ(std::string("d"), change.at(0).field_old_value.get<std::string>());
			ASSERT_EQ(std::string("q"), change.at(0).field_new_value.get<std::string>());

			ASSERT_EQ(std::string("c"), change.at(1).field_old_value.get<std::string>());
			ASSERT_EQ(std::string("w"), change.at(1).field_new_value.get<std::string>());

			session.save_changes();
		}
	}

	TEST_F(CrudTests, CrudCanUpdatePropertyToNull)
	{
		{
			auto session = test_suite_store->get().open_session();
			auto user = std::make_shared<NullableUser>();
			user->first_name = "user1";

			session.store(user, std::string("n_users/1"));
			session.save_changes();
		}
		{
			auto session = test_suite_store->get().open_session();
			auto user = session.load<NullableUser>("n_users/1");
			user->first_name.reset();
			session.save_changes();
		}
		{
			auto session = test_suite_store->get().open_session();
			auto user = session.load<NullableUser>("n_users/1");
			ASSERT_FALSE(user->first_name.has_value());
		}
	}

	TEST_F(CrudTests, CrudCanUpdatePropertyFromNullToObject)
	{
		{
			auto session = test_suite_store->get().open_session();
			auto poc = std::make_shared<Poc>();
			poc->name = "User";
			poc->obj.reset();

			session.store(poc, std::string("pocs/1"));
			session.save_changes();
		}
		{
			auto session = test_suite_store->get().open_session();
			auto poc = session.load<Poc>("pocs/1");
			ASSERT_FALSE(poc->obj.has_value());

			auto user = User();
			poc->obj = user;
			session.save_changes();
		}
		{
			auto session = test_suite_store->get().open_session();
			auto poc = session.load<Poc>("pocs/1");
			ASSERT_TRUE(poc->obj.has_value());
		}
	}
}
