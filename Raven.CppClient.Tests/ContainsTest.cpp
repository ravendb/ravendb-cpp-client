#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "EntityIdHelperUtil.h"

namespace contains_test
{
	struct UserWithFavs
	{
		std::string id{};
		std::string name{};
		std::vector<std::string> favourites{};
	};

	void to_json(nlohmann::json& j, const UserWithFavs& ivf)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "name", ivf.name);
		set_val_to_json(j, "favourites", ivf.favourites);
	}

	void from_json(const nlohmann::json& j, UserWithFavs& uvf)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "name", uvf.name);
		get_val_from_json(j, "favourites", uvf.favourites);
	}
}

namespace ravendb::client::tests::client
{
	class ContainsTest : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}

		static void SetUpTestCase()
		{
			REGISTER_ID_PROPERTY_FOR(contains_test::UserWithFavs, id);
		}
	};

	TEST_F(ContainsTest, ContainsTest)
	{
		auto store = get_document_store(TEST_NAME);
		
		{
			auto session = store->open_session();

			const auto user_creator = [&](std::string name, std::vector<std::string> favs)
			{
				auto user = std::make_shared<contains_test::UserWithFavs>();
				user->name = std::move(name);
				user->favourites = std::move(favs);

				session.store(user);
			};

			user_creator("John", { "java", "c#" });
			user_creator("Tarzan", { "java", "go" });
			user_creator("Jane", { "pascal" });

			session.save_changes();
		}
		{
			auto session = store->open_session();

			auto pascal_or_go_developer_names = session.query<contains_test::UserWithFavs>()
				->contains_any("favourites", std::vector<std::string>{"pascal", "go"})
				->select_fields<std::string>({ "name" })
				->to_list();

			ASSERT_EQ(2, pascal_or_go_developer_names.size());

			auto results_set = std::set<std::string>{ *pascal_or_go_developer_names.at(0),
				*pascal_or_go_developer_names.at(1) };

			auto expected_results = std::set<std::string>{ "Jane", "Tarzan" };

			ASSERT_EQ(expected_results, results_set);
		}
		{
			auto session = store->open_session();

			auto java_developers = session.query<contains_test::UserWithFavs>()
				->contains_all("favourites", std::vector<std::string>{"java"})
				->select_fields<std::string>({ "name" })
				->to_list();

			ASSERT_EQ(2, java_developers.size());

			auto results_set = std::set<std::string>{ *java_developers.at(0),
				*java_developers.at(1) };

			auto expected_results = std::set<std::string>{ "John", "Tarzan" };

			ASSERT_EQ(expected_results, results_set);
		}
	}
}
