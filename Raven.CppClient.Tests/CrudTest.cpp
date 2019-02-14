#include "pch.h"
//#define __USE_FIDDLER__
#include "TestSuiteBase.h"
#include "DocumentSession.h"
#include "RequestExecutor.h"
#include "User.h"
#include "Family.h"
#include "NullableUser.h"
#include "Arr.h"
#include "Poc.h"

namespace ravendb::client::tests::client
{
	class CrudTest : public infrastructure::TestSuiteBase
	{
	protected:
		static void SetUpTestCase()
		{
			test_suite_store = definitions::GET_DOCUMENT_STORE();
		}

		documents::session::DocumentInfo::ToJsonConverter arr2_to_json = [](std::shared_ptr<void> arr_param)->nlohmann::json
		{
			auto arr = std::static_pointer_cast<infrastructure::entities::Arr2>(arr_param);
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
			auto arr2 = std::make_shared<infrastructure::entities::Arr2>();
			const auto& json_array = static_cast<const nlohmann::json::array_t&>(json["arr1"]);
			arr2->arr1.reserve(json_array.size());
			for(const auto& str_arr : json_array)
			{
				infrastructure::entities::Arr1 arr1{};
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

	TEST_F(CrudTest, EntitiesAreSavedUsingLowerCase)
	{
		{
			auto session = test_suite_store->get().open_session();
			auto user1 = std::make_shared<infrastructure::entities::User>();
			user1->last_name = "user1";
			session.store(user1, "users/1");
			session.save_changes();
		}

		auto documents_command = documents::commands::GetDocumentsCommand("users/1", {}, false);
		test_suite_store->get().get_request_executor()->execute(documents_command);
		auto result = documents_command.get_result();

		auto user_json = result.results.at(0);
		ASSERT_TRUE(user_json.find("LastName") != user_json.end());

		{
			auto session = test_suite_store->get().open_session();
			auto users = session.advanced().raw_query("from Users where LastName = 'user1'")->to_list<infrastructure::entities::User>();

			ASSERT_EQ(1, users.size());
		}
	}


	//TODO implement
	TEST_F(CrudTest, CanCustomizePropertyNamingStrategy)
	{
		SUCCEED();
	}
	
	TEST_F(CrudTest, CrudOperations)
	{
		auto session = test_suite_store->get().open_session();
		auto user1 = std::make_shared<infrastructure::entities::User>();
		user1->last_name = "user1";
		session.store(user1, "users/1");
		
		auto user2 = std::make_shared<infrastructure::entities::User>();
		user2->name = "user2";
		user1->age = 1;
		session.store(user2, "users/2");

		auto user3 = std::make_shared<infrastructure::entities::User>();
		user3->name = "user3";
		user3->age = 1;
		session.store(user3, "users/3");

		auto user4 = std::make_shared<infrastructure::entities::User>();
		user4->name = "user4";
		session.store(user4, "users/4");

		session.delete_document(user2);
		user3->age = 3;
		session.save_changes();

		auto temp_user = session.load<infrastructure::entities::User>("users/2");
		ASSERT_FALSE(temp_user);

		temp_user = session.load<infrastructure::entities::User>("users/3");
		ASSERT_EQ(3, temp_user->age);

		user1 = session.load<infrastructure::entities::User>("users/1");
		user4 = session.load<infrastructure::entities::User>("users/4");

		session.delete_document(user4);
		user1->age = 10;
		session.save_changes();

		temp_user = session.load<infrastructure::entities::User>("users/4");
		ASSERT_FALSE(temp_user);

		temp_user = session.load<infrastructure::entities::User>("users/1");
		ASSERT_EQ(10, temp_user->age);		
	}

	TEST_F(CrudTest, CrudOperationsWithWhatChanged)
	{
		auto session = test_suite_store->get().open_session();
		auto user1 = std::make_shared<infrastructure::entities::User>();
		user1->last_name = "user1";
		session.store(user1, "users/1");

		auto user2 = std::make_shared<infrastructure::entities::User>();
		user2->name = "user2";
		user1->age = 1;
		session.store(user2, "users/2");

		auto user3 = std::make_shared<infrastructure::entities::User>();
		user3->name = "user3";
		user3->age = 1;
		session.store(user3, "users/3");

		auto user4 = std::make_shared<infrastructure::entities::User>();
		user4->name = "user4";
		session.store(user4, "users/4");

		session.delete_document(user2);
		user3->age = 3;

		ASSERT_EQ(4, session.advanced().what_changed().size());
		session.save_changes();

		auto temp_user = session.load<infrastructure::entities::User>("users/2");
		ASSERT_FALSE(temp_user);

		temp_user = session.load<infrastructure::entities::User>("users/3");
		ASSERT_EQ(3, temp_user->age);

		user1 = session.load<infrastructure::entities::User>("users/1");
		user4 = session.load<infrastructure::entities::User>("users/4");

		session.delete_document(user4);
		user1->age = 10;
		ASSERT_EQ(2, session.advanced().what_changed().size());

		session.save_changes();

		temp_user = session.load<infrastructure::entities::User>("users/4");
		ASSERT_FALSE(temp_user);

		temp_user = session.load<infrastructure::entities::User>("users/1");
		ASSERT_EQ(10, temp_user->age);
	}

	TEST_F(CrudTest, CrudOperationsWithArrayInObject)
	{
		auto session = test_suite_store->get().open_session();

		auto family = std::make_shared<infrastructure::entities::Family>();
		family->names = { "Hibernating Rhinos", "RavenDB" };
		session.store(family, "family/1");
		session.save_changes();

		auto new_family = session.load<infrastructure::entities::Family>("family/1");
		new_family->names = { "Toli", "Mitzi", "Boki" };
		ASSERT_EQ(1, session.advanced().what_changed().size());
		session.save_changes();
	}
	TEST_F(CrudTest, CrudOperationsWithArrayInObject2)
	{
		auto session = test_suite_store->get().open_session();

		auto family = std::make_shared<infrastructure::entities::Family>();
		family->names = { "Hibernating Rhinos", "RavenDB" };
		session.store(family, "family/1");
		session.save_changes();

		auto new_family = session.load<infrastructure::entities::Family>("family/1");
		new_family->names = { "Hibernating Rhinos", "RavenDB" };
		ASSERT_EQ(0, session.advanced().what_changed().size());

		new_family->names = { "RavenDB","Hibernating Rhinos" };
		ASSERT_EQ(1, session.advanced().what_changed().size());
		session.save_changes();
	}

	TEST_F(CrudTest, CrudOperationsWithArrayInObject3)
	{
		auto session = test_suite_store->get().open_session();

		auto family = std::make_shared<infrastructure::entities::Family>();
		family->names = { "Hibernating Rhinos", "RavenDB" };
		session.store(family, "family/1");
		session.save_changes();

		auto new_family = session.load<infrastructure::entities::Family>("family/1");
		new_family->names = { "RavenDB" };
		ASSERT_EQ(1, session.advanced().what_changed().size());
		session.save_changes();
	}

	TEST_F(CrudTest, CrudOperationsWithArrayInObject4)
	{
		auto session = test_suite_store->get().open_session();

		auto family = std::make_shared<infrastructure::entities::Family>();
		family->names = { "Hibernating Rhinos", "RavenDB" };
		session.store(family, "family/1");
		session.save_changes();

		auto new_family = session.load<infrastructure::entities::Family>("family/1");
		new_family->names = { "RavenDB", "Hibernating Rhinos", "Toli", "Mitzi", "Boki" };
		ASSERT_EQ(1, session.advanced().what_changed().size());
		session.save_changes();
	}

	TEST_F(CrudTest, CrudOperationsWithNull)
	{
		auto session = test_suite_store->get().open_session();

		auto user = std::make_shared<infrastructure::entities::NullableUser>();

		session.store(user, "n_users/1");
		session.save_changes();

		auto user2 = session.load<infrastructure::entities::NullableUser>("n_users/1");
		ASSERT_TRUE(session.advanced().what_changed().empty());

		user2->age = 3;
		ASSERT_EQ(1, session.advanced().what_changed().size());
	}

	TEST_F(CrudTest, CrudOperationsWithArrayOfObjects)
	{
		auto session = test_suite_store->get().open_session();

		auto member1 = std::make_shared<infrastructure::entities::Member>();
		member1->name = "Hibernating Rhinos";
		member1->age = 8;

		auto member2 = std::make_shared<infrastructure::entities::Member>();
		member2->name = "RavenDB";
		member2->age = 4;

		auto family = std::make_shared<infrastructure::entities::FamilyMembers>();
		family->members = { member1, member2 };

		session.store(family, "family_members/1");
		session.save_changes();

		member1 = std::make_shared<infrastructure::entities::Member>();
		member1->name = "RavenDB";
		member1->age = 4;

		member2 = std::make_shared<infrastructure::entities::Member>();
		member2->name = "Hibernating Rhinos";
		member2->age = 8;

		auto new_family = session.load<infrastructure::entities::FamilyMembers>("family_members/1");
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

		member1 = std::make_shared<infrastructure::entities::Member>();
		member1->name = "Toli";
		member1->age = 5;

		member2 = std::make_shared<infrastructure::entities::Member>();
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

	TEST_F(CrudTest, CrudOperationsWithArrayOfArrays)
	{
		{
			auto session = test_suite_store->get().open_session();

			infrastructure::entities::Arr1 a1 = { {"a", "b"} };
			infrastructure::entities::Arr1 a2 = { {"c", "d"} };
			auto arr = std::make_shared<infrastructure::entities::Arr2>();
			arr->arr1 = { a1, a2 };

			session.store(arr, "arr/1", arr2_to_json);
			session.save_changes();

			auto new_arr = session.load<infrastructure::entities::Arr2>("arr/1");

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

			auto new_arr = session.load<infrastructure::entities::Arr2>("arr/1", arr2_from_json, arr2_to_json);

			infrastructure::entities::Arr1 a1 = { {"q", "w"} };
			infrastructure::entities::Arr1 a2 = { {"a", "b"} };
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

	TEST_F(CrudTest, CrudCanUpdatePropertyToNull)
	{
		{
			auto session = test_suite_store->get().open_session();
			auto user = std::make_shared<infrastructure::entities::NullableUser>();
			user->first_name = "user1";

			session.store(user, "n_users/1");
			session.save_changes();
		}
		{
			auto session = test_suite_store->get().open_session();
			auto user = session.load<infrastructure::entities::NullableUser>("n_users/1");
			user->first_name.reset();
			session.save_changes();
		}
		{
			auto session = test_suite_store->get().open_session();
			auto user = session.load<infrastructure::entities::NullableUser>("n_users/1");
			ASSERT_FALSE(user->first_name.has_value());
		}
	}

	TEST_F(CrudTest, CrudCanUpdatePropertyFromNullToObject)
	{
		{
			auto session = test_suite_store->get().open_session();
			auto poc = std::make_shared<infrastructure::entities::Poc>();
			poc->name = "User";
			poc->obj.reset();

			session.store(poc, "pocs/1");
			session.save_changes();
		}
		{
			auto session = test_suite_store->get().open_session();
			auto poc = session.load<infrastructure::entities::Poc>("pocs/1");
			ASSERT_FALSE(poc->obj.has_value());

			auto user = infrastructure::entities::User();
			poc->obj = user;
			session.save_changes();
		}
		{
			auto session = test_suite_store->get().open_session();
			auto poc = session.load<infrastructure::entities::Poc>("pocs/1");
			ASSERT_TRUE(poc->obj.has_value());
		}
	}
}
