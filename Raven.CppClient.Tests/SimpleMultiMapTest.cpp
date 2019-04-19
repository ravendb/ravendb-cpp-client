#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "AbstractMultiMapIndexCreationTask.h"
#include "MaintenanceOperationExecutor.h"
#include "GetIndexOperation.h"

namespace simple_multi_map_test
{
	class CatsAndDogs : public ravendb::client::documents::indexes::AbstractMultiMapIndexCreationTask
	{
	public:
		~CatsAndDogs() override = default;
		CatsAndDogs()
		{
			SET_DEFAULT_INDEX_NAME;

			add_map("from cat in docs.Cats select new { cat.name }");
			add_map("from dog in docs.Dogs select new { dog.name }");
		}
	};

	class IHaveName
	{
	private:
		static std::shared_ptr<void> inner_from_json(const nlohmann::json& json);
	public:
		virtual ~IHaveName() = 0;
		virtual const std::string& get_name() const = 0;

		static ravendb::client::documents::session::DocumentInfo::FromJsonConverter from_json;
	};

	IHaveName::~IHaveName() = default;

	class Cat : public IHaveName
	{
	private:
		std::string name{};
	public:
		~Cat() override = default;
		const std::string& get_name() const override
		{
			return name;
		}
		void set_name(std::string name_param)
		{
			name = std::move(name_param);
		}
	};

	void from_json(const nlohmann::json& j, Cat& c)
	{
		c.set_name(j.at("name").get<std::string>());
	}

	void to_json(nlohmann::json& j, const Cat& c)
	{
		j["name"] = c.get_name();
	}

	class Dog : public IHaveName
	{
	private:
		std::string name{};
	public:
		~Dog() override = default;
		const std::string& get_name() const override
		{
			return name;
		}
		void set_name(std::string name_param)
		{
			name = std::move(name_param);
		}
	};

	void from_json(const nlohmann::json& j, Dog& d)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		d.set_name(j.at("name").get<std::string>());
	}

	void to_json(nlohmann::json& j, const Dog& d)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		j["name"] = d.get_name();
	}

	void to_json(nlohmann::json& j, const IHaveName& ihn)
	{
		if (const Cat* cat = dynamic_cast<const Cat*>(&ihn))
		{
			to_json(j, *cat);
		}
		if (const Dog* dog = dynamic_cast<const Dog*>(&ihn))
		{
			to_json(j, *dog);
		}
	}

	ravendb::client::documents::session::DocumentInfo::FromJsonConverter IHaveName::from_json = &IHaveName::inner_from_json;

	std::shared_ptr<void> IHaveName::inner_from_json(const nlohmann::json& json)
	{
		auto class_name = json[ravendb::client::constants::documents::metadata::KEY]
			[ravendb::client::constants::documents::metadata::RAVEN_CPP_TYPE].get<std::string>();

		if (class_name.find("Dog") != std::string::npos)
		{
			return ravendb::client::documents::session::DocumentInfo::default_from_json<Dog>(json);
		}
		if (class_name.find("Cat") != std::string::npos)
		{
			return ravendb::client::documents::session::DocumentInfo::default_from_json<Cat>(json);
		}

		return {};
	}
}

namespace ravendb::client::tests::client
{
	class SimpleMultiMapTest : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}
	};

	TEST_F(SimpleMultiMapTest, CanCreateMultiMapIndex)
	{
		auto store = get_document_store(TEST_NAME);

		simple_multi_map_test::CatsAndDogs().execute(store);

		auto index_definition = store->maintenance()->send(documents::operations::indexes::GetIndexOperation("CatsAndDogs"));
		ASSERT_EQ(2, index_definition->maps.size());
	}

	TEST_F(SimpleMultiMapTest, CanQueryUsingMultiMap)
	{
		auto store = get_document_store(TEST_NAME);

		simple_multi_map_test::CatsAndDogs().execute(store);
		{
			auto session = store->open_session();

			auto cat = std::make_shared<simple_multi_map_test::Cat>();
			cat->set_name("Tom");

			auto dog = std::make_shared<simple_multi_map_test::Dog>();
			dog->set_name("Oscar");

			session.store(cat);
			session.store(dog, "dogs/1");
			session.save_changes();
		}
		{
			auto session = store->open_session();

			auto have_names = session.query<simple_multi_map_test::IHaveName, simple_multi_map_test::CatsAndDogs>()
				->wait_for_non_stale_results(std::chrono::seconds(10))
				->order_by("name")
				->to_list(simple_multi_map_test::IHaveName::from_json);

			ASSERT_EQ(2, have_names.size());

			ASSERT_TRUE(std::dynamic_pointer_cast<simple_multi_map_test::Dog>(have_names[0]));
			ASSERT_TRUE(std::dynamic_pointer_cast<simple_multi_map_test::Cat>(have_names[1]));
		}
	}
}