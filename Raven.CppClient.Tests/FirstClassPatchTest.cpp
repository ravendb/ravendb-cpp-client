#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "AdvancedSessionOperations.h"
#include "User.h"

namespace first_class_patch_test
{
	struct Pet
	{
		std::string name{};
		std::string kind{};
	};

	void to_json(nlohmann::json& j, const Pet& p)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Name", p.name);
		set_val_to_json(j, "Kind", p.kind);
	}

	void from_json(const nlohmann::json& j, Pet& p)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Name", p.name);
		get_val_from_json(j, "Kind", p.kind);
	}

	struct Friend
	{
		std::string name{};
		int32_t age{};
		Pet pet{};
	};

	void to_json(nlohmann::json& j, const Friend& f)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Name", f.name);
		set_val_to_json(j, "Age", f.age);
		set_val_to_json(j, "Pet", f.pet);
	}

	void from_json(const nlohmann::json& j, Friend& f)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Name", f.name);
		get_val_from_json(j, "Age", f.age);
		get_val_from_json(j, "Pet", f.pet);
	}

	struct Stuff
	{
		int32_t key{};
		std::string phone{};
		Pet pet{};
		Friend the_friend{};
		std::unordered_map<std::string, std::string> dic{};
	};

	void to_json(nlohmann::json& j, const Stuff& s)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Key", s.key);
		set_val_to_json(j, "Phone", s.phone);
		set_val_to_json(j, "Pet", s.pet);
		set_val_to_json(j, "Friend", s.the_friend);
		set_val_to_json(j, "Dic", s.dic);
	}

	void from_json(const nlohmann::json& j, Stuff& s)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Key", s.key);
		get_val_from_json(j, "Phone", s.phone);
		get_val_from_json(j, "Pet", s.pet);
		get_val_from_json(j, "Friend", s.the_friend);
		get_val_from_json(j, "Dic", s.dic);
	}

	struct User
	{
		std::vector<Stuff> stuff{};
		DateTimeOffset last_login{};
		std::vector<int32_t> numbers{};
	};

	void to_json(nlohmann::json& j, const User& u)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Stuff", u.stuff);
		set_val_to_json(j, "LastLogin", u.last_login);
		set_val_to_json(j, "Numbers", u.numbers);
	}

	void from_json(const nlohmann::json& j, User& u)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Stuff", u.stuff);
		get_val_from_json(j, "LastLogin", u.last_login);
		get_val_from_json(j, "Numbers", u.numbers);
	}
}

namespace ravendb::client::tests::client
{
	class FirstClassPatchTest : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}
	};

	TEST_F(FirstClassPatchTest, CanPatch)
	{
		auto store = get_document_store(TEST_NAME);

		std::string user_id{};
		std::vector<first_class_patch_test::Stuff> stuff{3};
		stuff[0].key = 6;

		auto user = std::make_shared<first_class_patch_test::User>();
		user->numbers.push_back(66);
		user->stuff = std::move(stuff);
		{
			auto session = store->open_session();
			session.store(user);
			user_id = *session.advanced().get_document_id(user);
			session.save_changes();
		}

		const auto time_point = DateTimeOffset("2019-02-12T08:47:17.5033415Z");

		{
			auto session = store->open_session();

			session.advanced().patch(user_id,
				"Numbers[0]", 31);
			session.advanced().patch(user_id,
				"LastLogin", time_point);
			session.save_changes();
		}
		{
			auto session = store->open_session();
			auto loaded = session.load<first_class_patch_test::User>(user_id);
			ASSERT_EQ(31, loaded->numbers[0]);
			ASSERT_EQ(time_point.to_string(), loaded->last_login.to_string());

			session.advanced().patch(loaded, "Stuff[0].Phone", "123456");
			session.save_changes();
		}
		{
			auto session = store->open_session();
			auto loaded = session.load<first_class_patch_test::User>(user_id);
			ASSERT_EQ("123456", loaded->stuff[0].phone);
		}
	}

	TEST_F(FirstClassPatchTest, ThrowOnPatchAndModify)
	{
		auto store = get_document_store(TEST_NAME);

		std::string user_id{};
		auto user = std::make_shared<first_class_patch_test::User>();
		user->numbers.push_back(66);
		{
			auto session = store->open_session();
			session.store(user);
			user_id = *session.advanced().get_document_id(user);
			session.save_changes();
		}
		{
			auto session = store->open_session();
			auto loaded = session.load<first_class_patch_test::User>(user_id);
			loaded->numbers[0] = 1;
			session.advanced().patch(loaded, "Numbers[0]", 2);
			try
			{
				session.save_changes();
			}
			catch (std::runtime_error& e)
			{
				std::ostringstream msg{};
				msg << "Cannot perform save because document " << user_id <<
					" has been modified by the session" <<
					" and is also taking part in deferred PATCH command";
				ASSERT_EQ(msg.str(), std::string(e.what()));

				SUCCEED();
				return;
			}
			FAIL();
		}
	}

	TEST_F(FirstClassPatchTest, CanPatchComplex)
	{
		auto store = get_document_store(TEST_NAME);

		std::string user_id{};
		std::vector<first_class_patch_test::Stuff> stuff{3};
		stuff[0].key = 6;

		auto user = std::make_shared<first_class_patch_test::User>();
		user->stuff = std::move(stuff);

		{
			auto session = store->open_session();
			session.store(user);
			user_id = *session.advanced().get_document_id(user);
			session.save_changes();
		}
		{
			auto session = store->open_session();
			auto new_stuff = first_class_patch_test::Stuff();
			new_stuff.key = 4;
			new_stuff.phone = "9255864406";
			session.advanced().patch(user_id,
				"Stuff[1]", new_stuff);
			session.save_changes();
		}
		{
			auto session = store->open_session();
			auto loaded = session.load<first_class_patch_test::User>(user_id);
			ASSERT_EQ("9255864406", loaded->stuff[1].phone);
			ASSERT_EQ(4, loaded->stuff[1].key);
			ASSERT_TRUE(loaded->stuff[1].the_friend.name.empty());

			auto pet1 = first_class_patch_test::Pet();
			pet1.kind = "Dog";
			pet1.name = "Tuzik";

			auto friends_pet = first_class_patch_test::Pet();
			friends_pet.kind = "Cat";
			friends_pet.name = "Boris";

			auto the_friend = first_class_patch_test::Friend();
			the_friend.name = "Tomagochi";
			the_friend.age = 5;
			the_friend.pet = std::move(friends_pet);

			auto second_stuff = first_class_patch_test::Stuff();
			second_stuff.key = 4;
			second_stuff.phone = "9255864406";
			second_stuff.pet = std::move(pet1);
			second_stuff.the_friend = std::move(the_friend);

			auto map = std::unordered_map<std::string, std::string>();
			map.insert_or_assign("Ohio", "Columbus");
			map.insert_or_assign("Utah", "Salt Lake City");
			map.insert_or_assign("Texas", "Austin");
			map.insert_or_assign("California", "Sacramento");

			second_stuff.dic = std::move(map);

			session.advanced().patch(loaded, "Stuff[2]", second_stuff);
			session.save_changes();
		}
		{
			auto session = store->open_session();
			auto loaded = session.load<first_class_patch_test::User>(user_id);

			ASSERT_EQ("Tuzik", loaded->stuff[2].pet.name);
			ASSERT_EQ("Tomagochi", loaded->stuff[2].the_friend.name);
			ASSERT_EQ("Boris", loaded->stuff[2].the_friend.pet.name);
			ASSERT_EQ(4, loaded->stuff[2].dic.size());
			ASSERT_EQ("Salt Lake City", loaded->stuff[2].dic.at("Utah"));
		}
	}

	//TODO use id generator
	TEST_F(FirstClassPatchTest, CanAddToArray)
	{
		auto store = get_document_store(TEST_NAME);

		std::string user_id{};
		std::vector<first_class_patch_test::Stuff> stuff(1);
		stuff[0].key = 6;

		auto user = std::make_shared<first_class_patch_test::User>();
		user->stuff = stuff;
		user->numbers = { 1,2 };

		{
			auto session = store->open_session();
			session.store(user);
			user_id = *session.advanced().get_document_id(user);
			session.save_changes();
		}
		{
			auto session = store->open_session();
			session.advanced().patch(user_id, "Numbers",
				std::function<void(documents::session::JavaScriptArray<int32_t>&)>(
					[](documents::session::JavaScriptArray<int32_t>& arr)->void
			{
				arr.add(3);
			}));
			session.advanced().patch(user_id, "Stuff",
				std::function<void(documents::session::JavaScriptArray<first_class_patch_test::Stuff>&)>(
					[](documents::session::JavaScriptArray<first_class_patch_test::Stuff>& arr)->void
			{
				first_class_patch_test::Stuff stuff1{};
				stuff1.key = 75;
				arr.add(stuff1);
			}));
			session.save_changes();
		}
		{
			auto session = store->open_session();
			auto loaded = session.load<first_class_patch_test::User>(user_id);

			ASSERT_EQ(3, loaded->numbers[2]);
			ASSERT_EQ(75, loaded->stuff[1].key);

			session.advanced().patch(loaded, "Numbers",
				std::function<void(documents::session::JavaScriptArray<int32_t>&)>(
					[](documents::session::JavaScriptArray<int32_t>& arr)->void
			{
				arr.add({ 101, 102, 103 });
			}));
			session.advanced().patch(user_id, "Stuff",
				std::function<void(documents::session::JavaScriptArray<first_class_patch_test::Stuff>&)>(
					[](documents::session::JavaScriptArray<first_class_patch_test::Stuff>& arr)->void
			{
				first_class_patch_test::Stuff s1{};
				s1.key = 102;
				first_class_patch_test::Stuff s2{};
				s2.phone = "123456";

				arr.add(s1).add(s2);
			}));
			session.save_changes();
		}
		{
			auto session = store->open_session();
			auto loaded = session.load<first_class_patch_test::User>(user_id);
			ASSERT_EQ(6, loaded->numbers.size());
			ASSERT_EQ(103, loaded->numbers[5]);

			ASSERT_EQ(102, loaded->stuff[2].key);
			ASSERT_EQ("123456", loaded->stuff[3].phone);

			session.advanced().patch(loaded, "Numbers",
				std::function<void(documents::session::JavaScriptArray<int32_t>&)>(
					[](documents::session::JavaScriptArray<int32_t>& arr)->void
			{
				arr.add({ 201, 202, 203 });
			}));
			session.save_changes();
		}
		{
			auto session = store->open_session();
			auto loaded = session.load<first_class_patch_test::User>(user_id);
			ASSERT_EQ(9, loaded->numbers.size());
			ASSERT_EQ(202, loaded->numbers[7]);
		}
	}

	TEST_F(FirstClassPatchTest, CanRemoveFromArray)
	{
		auto store = get_document_store(TEST_NAME);

		std::string user_id{};
		std::vector<first_class_patch_test::Stuff> stuff{ 2 };
		stuff[0].key = 6;
		stuff[1].phone = "123456";

		auto user = std::make_shared<first_class_patch_test::User>();
		user->stuff = stuff;
		user->numbers = { 1, 2, 3 };

		{
			auto session = store->open_session();
			session.store(user);
			user_id = *session.advanced().get_document_id(user);
			session.save_changes();
		}
		{
			auto session = store->open_session();
			session.advanced().patch(user_id, "Numbers",
				std::function<void(documents::session::JavaScriptArray<int32_t>&)>(
					[](documents::session::JavaScriptArray<int32_t>& arr)->void
			{
				arr.remove_at(1);
			}));
			session.advanced().patch(user_id, "Stuff",
				std::function<void(documents::session::JavaScriptArray<first_class_patch_test::Stuff>&)>(
					[](documents::session::JavaScriptArray<first_class_patch_test::Stuff>& arr)->void
			{
				arr.remove_at(0);
			}));
			session.save_changes();
		}
		{
			auto session = store->open_session();
			auto loaded = session.load<first_class_patch_test::User>(user_id);

			ASSERT_EQ(2, loaded->numbers.size());
			ASSERT_EQ(3, loaded->numbers[1]);
			ASSERT_EQ(1, loaded->stuff.size());
			ASSERT_EQ("123456", loaded->stuff[0].phone);
		}
	}

	TEST_F(FirstClassPatchTest, CanIncrement)
	{
		auto store = get_document_store(TEST_NAME);

		std::string user_id{};
		auto stuff = std::vector<first_class_patch_test::Stuff>(3);
		stuff[0].key = 6;

		auto user = std::make_shared<first_class_patch_test::User>();
		user->stuff = stuff;
		user->numbers = { 66 };

		{
			auto session = store->open_session();
			session.store(user);
			user_id = *session.advanced().get_document_id(user);
			session.save_changes();
		}
		{
			auto session = store->open_session();
			session.advanced().increment(user_id,
				"Numbers[0]", 1);
			session.save_changes();
		}
		{
			auto session = store->open_session();
			auto loaded = session.load<first_class_patch_test::User>(user_id);
			ASSERT_EQ(67, loaded->numbers[0]);

			session.advanced().increment(loaded, "Stuff[0].Key", -3);
			session.save_changes();
		}
		{
			auto session = store->open_session();
			auto loaded = session.load<first_class_patch_test::User>(user_id);
			ASSERT_EQ(3, loaded->stuff[0].key);
		}
	}

	TEST_F(FirstClassPatchTest, ShouldMergePatchCalls)
	{
		auto store = get_document_store(TEST_NAME);

		std::string user_id{};
		auto stuff = std::vector<first_class_patch_test::Stuff>(3);
		stuff[0].key = 6;

		auto user = std::make_shared<first_class_patch_test::User>();
		user->stuff = stuff;
		user->numbers.push_back(66);

		auto user2 = std::make_shared<first_class_patch_test::User>();
		user2->numbers = { 1, 2,3 };
		user2->stuff = stuff;

		std::string user2_id{};

		{
			auto session = store->open_session();
			session.store(user);
			user_id = *session.advanced().get_document_id(user);
			session.store(user2);
			user2_id = *session.advanced().get_document_id(user2);
			session.save_changes();
		}

		const auto time_point = DateTimeOffset("2019-02-12T12:15:25.1234000");

		{
			auto session = store->open_session();
			session.advanced().patch(user_id,
				"Numbers[0]", 31);
			ASSERT_EQ(1, session.get_session_implementation()->get_deferred_commands_count());

			session.advanced().patch(user_id,
				"LastLogin", time_point);
			ASSERT_EQ(1, session.get_session_implementation()->get_deferred_commands_count());

			session.advanced().patch(user2_id,
				"Numbers[0]", 123);
			ASSERT_EQ(2, session.get_session_implementation()->get_deferred_commands_count());

			session.advanced().patch(user2_id,
				"LastLogin", time_point);
			ASSERT_EQ(2, session.get_session_implementation()->get_deferred_commands_count());

			session.save_changes();
		}
		{
			auto session = store->open_session();
			session.advanced().increment(user_id,
				"Numbers[0]", 1);
			ASSERT_EQ(1, session.get_session_implementation()->get_deferred_commands_count());

			session.advanced().patch<int32_t>(user_id,
				"Numbers", [](documents::session::JavaScriptArray<int32_t>& arr)->void
			{
				arr.add(77);
			});
			ASSERT_EQ(1, session.get_session_implementation()->get_deferred_commands_count());

			session.advanced().patch<int32_t>(user_id,
				"Numbers", [](documents::session::JavaScriptArray<int32_t>& arr)->void
			{
				arr.add(88);
			});
			ASSERT_EQ(1, session.get_session_implementation()->get_deferred_commands_count());

			std::function<void(documents::session::JavaScriptArray<int32_t>&)> array_adder = []
			(documents::session::JavaScriptArray<int32_t>& arr) -> void
			{
				arr.remove_at(1);
			};

			session.advanced().patch(user_id,
				"Numbers", array_adder);
			ASSERT_EQ(1, session.get_session_implementation()->get_deferred_commands_count());

			session.save_changes();
		}
	}

	TEST_F(FirstClassPatchTest, CanUpdateSessionByPatch)
	{
		auto store = get_document_store(TEST_NAME);

		std::string user_id{};
		std::vector<first_class_patch_test::Stuff> stuff{ 3 };
		stuff[0].key = 6;

		auto user = std::make_shared<first_class_patch_test::User>();
		user->stuff = std::move(stuff);

		{
			auto session = store->open_session();
			session.store(user);
			user_id = *session.advanced().get_document_id(user);
			session.save_changes();
		}
		{
			auto session = store->open_session();
			auto loaded = session.load<first_class_patch_test::User>(user_id);

			auto new_stuff = first_class_patch_test::Stuff();
			new_stuff.key = 4;
			new_stuff.phone = "9255864406";
			session.advanced().patch(user_id,
				"Stuff[1]", new_stuff);
			session.save_changes();

			ASSERT_EQ("9255864406", loaded->stuff[1].phone);
		}
		{
			auto session = store->open_session();
			auto loaded = session.load<first_class_patch_test::User>(user_id);
			ASSERT_EQ("9255864406", loaded->stuff[1].phone);
			ASSERT_EQ(4, loaded->stuff[1].key);
			ASSERT_TRUE(loaded->stuff[1].the_friend.name.empty());

			auto pet1 = first_class_patch_test::Pet();
			pet1.kind = "Dog";
			pet1.name = "Tuzik";

			auto friends_pet = first_class_patch_test::Pet();
			friends_pet.kind = "Cat";
			friends_pet.name = "Boris";

			auto the_friend = first_class_patch_test::Friend();
			the_friend.name = "Tomagochi";
			the_friend.age = 5;
			the_friend.pet = std::move(friends_pet);

			auto second_stuff = first_class_patch_test::Stuff();
			second_stuff.key = 4;
			second_stuff.phone = "9255864406";
			second_stuff.pet = std::move(pet1);
			second_stuff.the_friend = std::move(the_friend);

			auto map = std::unordered_map<std::string, std::string>();
			map.insert_or_assign("Ohio", "Columbus");
			map.insert_or_assign("Utah", "Salt Lake City");
			map.insert_or_assign("Texas", "Austin");
			map.insert_or_assign("California", "Sacramento");

			second_stuff.dic = std::move(map);

			session.advanced().patch(loaded, "Stuff[2]", second_stuff);
			session.save_changes();

			ASSERT_EQ("Tuzik", loaded->stuff[2].pet.name);
			ASSERT_EQ("Tomagochi", loaded->stuff[2].the_friend.name);
			ASSERT_EQ("Cat", loaded->stuff[2].the_friend.pet.kind);
			ASSERT_EQ(4, loaded->stuff[2].dic.size());
			ASSERT_EQ("Austin", loaded->stuff[2].dic.at("Texas"));
		}
	}
}