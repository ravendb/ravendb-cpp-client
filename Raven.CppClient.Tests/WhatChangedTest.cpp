#include "pch.h"
//#define __USE_FIDDLER__
#include "TestSuiteBase.h"
#include "DocumentSession.h"
#include "AdvancedSessionOperations.h"
#include "json_utils.h"
#include "User.h"

namespace what_changed_test
{
	struct BasicName
	{
		std::string name{};
	};

	inline void to_json(nlohmann::json& j, const BasicName& bn)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Name", bn.name);
	}

	inline void from_json(const nlohmann::json& j, BasicName& bn)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Name", bn.name);
	}

	struct NameAndAge
	{
		std::string name{};
		int32_t age{};
	};

	inline void to_json(nlohmann::json& j, const NameAndAge& naa)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Name", naa.name);
		set_val_to_json(j, "Age", naa.age);
	}

	inline void from_json(const nlohmann::json& j, NameAndAge& naa)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Name", naa.name);
		get_val_from_json(j, "Age", naa.age);
	}

	struct BasicAge
	{
		int32_t age;
	};

	inline void to_json(nlohmann::json& j, const BasicAge& ba)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Age", ba.age);
	}

	inline void from_json(const nlohmann::json& j, BasicAge& ba)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Age", ba.age);
	}

	struct Integer
	{
		int32_t number{};
	};

	inline void to_json(nlohmann::json& j, const Integer& i)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Number", i.number);
	}

	inline void from_json(const nlohmann::json& j, Integer& i)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Number", i.number);
	}

	struct Double
	{
		double number{};
	};

	inline void to_json(nlohmann::json& j, const Double& d)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Number", d.number);
	}

	inline void from_json(const nlohmann::json& j, Double& d)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Number", d.number);
	}

	struct Arr
	{
		std::vector<std::any> array{};
	};

	inline void to_json(nlohmann::json& j, const Arr& arr)
	{
		j["Array"] = nlohmann::json::array();

		for (const auto& val : arr.array)
		{
			if (auto val_ptr = std::any_cast<std::string>(&val);
				nullptr != val_ptr)
			{
				j["Array"].push_back(*val_ptr);
				continue;
			} 
			if (auto val_ptr = std::any_cast<int32_t>(&val);
				nullptr != val_ptr)
			{
				j["Array"].push_back(*val_ptr);
				continue;
			}
			if (auto val_ptr = std::any_cast<double>(&val);
				nullptr != val_ptr)
			{
				j["Array"].push_back(*val_ptr);
				continue;
			}
			throw std::invalid_argument("Unknown type");
		}
	}

	inline void from_json(const nlohmann::json& j, Arr& arr)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		for (const auto& val : j.at("Array"))
		{
			if (val.is_string())
			{
				arr.array.push_back(val.get<std::string>());
				continue;
			}
			if (val.is_number_integer())
			{
				arr.array.push_back(val.get<int32_t>());
				continue;
			}
			if (val.is_number_float())
			{
				arr.array.push_back(val.get<double>());
				continue;
			}
			throw std::invalid_argument("Unknown type or null");
		}
	}
}

namespace ravendb::client::tests::client
{
	class WhatChangedTest : public infrastructure::TestSuiteBase
	{
	protected:
		static void SetUpTestCase()
		{
			test_suite_store = definitions::GET_DOCUMENT_STORE();
		}
	};

	TEST_F(WhatChangedTest, WhatChangedNewField)
	{
		{
			auto session = test_suite_store->get().open_session();

			auto basic_name = std::make_shared<what_changed_test::BasicName>();
			basic_name->name = "Alexander";
			session.store(basic_name, "users/1");

			ASSERT_EQ(1, session.advanced().what_changed().size());
			session.save_changes();
		}
		{
			auto session = test_suite_store->get().open_session();

			auto user = session.load<what_changed_test::NameAndAge>("users/1");
			user->age = 38;
			auto changes = session.advanced().what_changed();
			ASSERT_EQ(1, changes.at("users/1").size());
			ASSERT_EQ(documents::session::DocumentsChanges::ChangeType::NEW_FIELD,
				changes.at("users/1").at(0).change);
			session.save_changes();
		}
	}

	TEST_F(WhatChangedTest, WhatChangedRemovedField)
	{
		{
			auto session = test_suite_store->get().open_session();

			auto name_and_age = std::make_shared<what_changed_test::NameAndAge>();
			name_and_age->age = 38;
			name_and_age->name = "Alexander";

			session.store(name_and_age, "users/2");
			ASSERT_EQ(1, session.advanced().what_changed().size());
			session.save_changes();
		}
		{
			auto session = test_suite_store->get().open_session();

			auto basic_age = session.load<what_changed_test::BasicAge>("users/2");
			auto changes = session.advanced().what_changed();

			ASSERT_EQ(1, changes.at("users/2").size());
			ASSERT_EQ(documents::session::DocumentsChanges::ChangeType::REMOVED_FIELD,
				changes.at("users/2").at(0).change);
		}
	}

	TEST_F(WhatChangedTest, WhatChangedChangeField)
	{
		{
			auto session = test_suite_store->get().open_session();

			auto basic_age = std::make_shared<what_changed_test::BasicAge>();
			basic_age->age = 38;
			session.store(basic_age, "users/3");

			ASSERT_EQ(1, session.advanced().what_changed().size());
			session.save_changes();
		}
		{
			auto session = test_suite_store->get().open_session();

			auto integer = session.load<what_changed_test::Integer>("users/3");
			auto changes = session.advanced().what_changed();

			ASSERT_EQ(2, changes.at("users/3").size());
			ASSERT_EQ(documents::session::DocumentsChanges::ChangeType::REMOVED_FIELD,
				changes.at("users/3").at(0).change);
			ASSERT_EQ(documents::session::DocumentsChanges::ChangeType::NEW_FIELD,
				changes.at("users/3").at(1).change);
			session.save_changes();
		}
	}

	TEST_F(WhatChangedTest, WhatChangedArrayValueChanged)
	{
		{
			auto session = test_suite_store->get().open_session();

			auto arr = std::make_shared<what_changed_test::Arr>();
			arr->array = { std::string("a"), int32_t(1), std::string("b") };
			session.store(arr, "users/4");
			auto changes = session.advanced().what_changed();

			ASSERT_EQ(1, changes.size());
			ASSERT_EQ(1, changes.at("users/4").size());
			ASSERT_EQ(documents::session::DocumentsChanges::ChangeType::DOCUMENT_ADDED,
				changes.at("users/4").at(0).change);

			session.save_changes();
		}
		{
			auto session = test_suite_store->get().open_session();

			auto arr = session.load<what_changed_test::Arr>("users/4");
			arr->array = { std::string("a"), int32_t(2), std::string("c") };

			auto changes = session.advanced().what_changed();
			ASSERT_EQ(1, changes.size());
			ASSERT_EQ(2, changes.at("users/4").size());

			ASSERT_EQ(documents::session::DocumentsChanges::ChangeType::ARRAY_VALUE_CHANGED,
				changes.at("users/4").at(0).change);
			ASSERT_EQ(std::string("1"), changes.at("users/4").at(0).field_old_value.dump());
			ASSERT_EQ(2, changes.at("users/4").at(0).field_new_value.get<int32_t>());

			ASSERT_EQ(documents::session::DocumentsChanges::ChangeType::ARRAY_VALUE_CHANGED,
				changes.at("users/4").at(1).change);
			ASSERT_EQ(std::string("\"b\""), changes.at("users/4").at(1).field_old_value.dump());
			ASSERT_EQ(std::string("c"), changes.at("users/4").at(1).field_new_value.get<std::string>());

		}
	}

	TEST_F(WhatChangedTest, WhatChangedArrayValueAdded)
	{
		{
			auto session = test_suite_store->get().open_session();

			auto arr = std::make_shared<what_changed_test::Arr>();
			arr->array = { std::string("a"), int32_t(1), std::string("b") };
			session.store(arr, "arrs/1");
			session.save_changes();
		}
		{
			auto session = test_suite_store->get().open_session();

			auto arr = session.load<what_changed_test::Arr>("arrs/1");
			arr->array = { std::string("a"), int32_t(1), std::string("b"), std::string("c"), int32_t(2) };

			auto changes = session.advanced().what_changed();

			ASSERT_EQ(1, changes.size());
			ASSERT_EQ(2, changes.at("arrs/1").size());

			ASSERT_EQ(documents::session::DocumentsChanges::ChangeType::ARRAY_VALUE_ADDED,
				changes.at("arrs/1").at(0).change);
			ASSERT_EQ("\"c\"", changes.at("arrs/1").at(0).field_new_value.dump());
			ASSERT_TRUE(changes.at("arrs/1").at(0).field_old_value.is_null());

			ASSERT_EQ(documents::session::DocumentsChanges::ChangeType::ARRAY_VALUE_ADDED,
				changes.at("arrs/1").at(1).change);
			ASSERT_EQ(2, changes.at("arrs/1").at(1).field_new_value.get<int32_t>());
			ASSERT_TRUE(changes.at("arrs/1").at(1).field_old_value.is_null());
		}
	}

	TEST_F(WhatChangedTest, WhatChangedArrayValueRemoved)
	{
		{
			auto session = test_suite_store->get().open_session();

			auto arr = std::make_shared<what_changed_test::Arr>();
			arr->array = { std::string("a"), int32_t(1), std::string("b") };
			session.store(arr, "arr/1");
			session.save_changes();
		}
		{
			auto session = test_suite_store->get().open_session();

			auto arr = session.load<what_changed_test::Arr>("arr/1");
			arr->array = { std::string("a") };

			auto changes = session.advanced().what_changed();

			ASSERT_EQ(1, changes.size());
			ASSERT_EQ(2, changes.at("arr/1").size());

			ASSERT_EQ(documents::session::DocumentsChanges::ChangeType::ARRAY_VALUE_REMOVED,
				changes.at("arr/1").at(0).change);
			ASSERT_EQ(1, changes.at("arr/1").at(0).field_old_value.get<int32_t>());
			ASSERT_TRUE(changes.at("arr/1").at(0).field_new_value.is_null());

			ASSERT_EQ(documents::session::DocumentsChanges::ChangeType::ARRAY_VALUE_REMOVED,
				changes.at("arr/1").at(1).change);
			ASSERT_EQ("\"b\"", changes.at("arr/1").at(1).field_old_value.dump());
			ASSERT_TRUE(changes.at("arr/1").at(1).field_new_value.is_null());
		}
	}

	TEST_F(WhatChangedTest, ravenDB_8169)
		//Test that when old and new values are of different type
		//but have the same value, we consider them unchanged
	{
		{
			auto session = test_suite_store->get().open_session();

			auto an_int = std::make_shared<what_changed_test::Integer>();
			an_int->number = 1;
			session.store(an_int, "num/1");

			auto a_double = std::make_shared<what_changed_test::Double>();
			a_double->number = 2.0;
			session.store(a_double, "num/2");

			session.save_changes();
		}
		{
			auto session = test_suite_store->get().open_session();

			session.load<what_changed_test::Double>("num/1");
			ASSERT_TRUE(session.advanced().what_changed().empty());
		}
		{
			auto session = test_suite_store->get().open_session();

			session.load<what_changed_test::Integer>("num/2");
			ASSERT_TRUE(session.advanced().what_changed().empty());
		}
	}

	TEST_F(WhatChangedTest, WhatChangedShouldBeIdempotentOperation)
	{
		auto session = test_suite_store->get().open_session();

		auto user1 = std::make_shared<infrastructure::entities::User>();
		user1->name = "user1";

		auto user2 = std::make_shared<infrastructure::entities::User>();
		user2->name = "user2";
		user2->age = 1;

		auto user3 = std::make_shared<infrastructure::entities::User>();
		user3->name = "user3";
		user3->age = 1;

		session.store(user1, "user/1");
		session.store(user2, "user/2");
		session.store(user3, "user/3");

		ASSERT_EQ(3, session.advanced().what_changed().size());

		session.save_changes();

		user1 = session.load<infrastructure::entities::User>("user/1");
		user2 = session.load<infrastructure::entities::User>("user/2");

		user1->age = 10;
		session.delete_document(user2);

		ASSERT_EQ(2, session.advanced().what_changed().size());
		ASSERT_EQ(2, session.advanced().what_changed().size());
	}

	TEST_F(WhatChangedTest, WhatChangedHasChanges)
	{
		{
			auto session = test_suite_store->get().open_session();

			auto user1 = std::make_shared<infrastructure::entities::User>();
			user1->name = "user11";

			auto user2 = std::make_shared<infrastructure::entities::User>();
			user2->name = "user12";
			user2->age = 1;

			session.store(user1, "user/11");
			session.store(user2, "user/12");
			session.save_changes();
		}
		{
			auto session = test_suite_store->get().open_session();

			ASSERT_FALSE(session.advanced().has_changes());

			auto u1 = session.load<infrastructure::entities::User>("user/11");
			auto u2 = session.load<infrastructure::entities::User>("user/12");

			ASSERT_FALSE(session.advanced().has_changed(u1));
			ASSERT_FALSE(session.advanced().has_changed(u2));

			u1->name = "new name";

			ASSERT_TRUE(session.advanced().has_changed(u1));
			ASSERT_FALSE(session.advanced().has_changed(u2));
			ASSERT_TRUE(session.advanced().has_changes());
		}
	}
}