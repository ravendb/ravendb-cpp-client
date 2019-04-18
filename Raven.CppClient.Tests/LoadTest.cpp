#include "pch.h"
#include "RavenTestDriver.h"
#include "DocumentSession.h"
#include "AdvancedSessionOperations.h"
#include "raven_test_definitions.h"

namespace load_test
{
	struct Foo
	{
		std::string name{};
	};

	inline void to_json(nlohmann::json& j, const Foo& f)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Name", f.name);
	}

	inline void from_json(const nlohmann::json& j, Foo& f)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Name", f.name);
	}

	struct Bar
	{
		std::string foo_id{};
		std::vector<std::string> foo_ids{};
		std::string name{};
	};


	inline void to_json(nlohmann::json& j, const Bar& b)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "FooId", b.foo_id);
		set_val_to_json(j, "FooIds", b.foo_ids);
		set_val_to_json(j, "Name", b.name);
	}

	inline void from_json(const nlohmann::json& j, Bar& b)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "FooId", b.foo_id);
		get_val_from_json(j, "FooIds", b.foo_ids);
		get_val_from_json(j, "Name", b.name);
	}

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
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		c.set_name(j.at("name").get<std::string>());
	}

	void to_json(nlohmann::json& j, const Cat& c)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

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

namespace ravendb::client::tests::client::documents
{
	class LoadTest : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<ravendb::client::documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}
	};

	TEST_F(LoadTest, LoadWithIncludes)
	{
		std::string bar_id{};
		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();
			auto foo = std::make_shared<load_test::Foo>();
			foo->name = "Beginning";
			session.store(foo);

			auto foo_id = *session.advanced().get_document_id(foo);
			auto bar = std::make_shared<load_test::Bar>();
			bar->name = "End";
			bar->foo_id = foo_id;
			session.store(bar);

			bar_id = *session.advanced().get_document_id(bar);
			session.save_changes();
		}
		{
			auto session = store->open_session();
			std::vector<std::string> temp_ids = { bar_id };
			auto bar = session.include("FooId").load<load_test::Bar>(temp_ids.begin(), temp_ids.end());

			ASSERT_EQ(1, bar.size());
			ASSERT_TRUE(bar.find(bar_id) != bar.end());

			auto num_of_requests = session.advanced().get_number_of_requests();

			auto foo = session.load<load_test::Foo>(bar.at(bar_id)->foo_id);

			ASSERT_TRUE(foo);
			ASSERT_EQ(std::string("Beginning"), foo->name);
			ASSERT_EQ(num_of_requests, session.advanced().get_number_of_requests());
		}
	}

	//TODO this test MAY be influenced from non-implemented feature(IncludesUtils = include path parsing)
	TEST_F(LoadTest, LoadWithIncludesAndMissingDocument)
	{
		std::string bar_id{};
		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();
			auto bar = std::make_shared<load_test::Bar>();
			bar->name = "End";
			bar->foo_id = "non_exists/1";
			session.store(bar);
			bar_id = *session.advanced().get_document_id(bar);
			session.save_changes();
		}
		{
			auto session = store->open_session();
			std::vector<std::string> temp_ids = { bar_id };
			auto bar = session.include("FooId").load<load_test::Bar>(temp_ids.begin(), temp_ids.end());

			ASSERT_EQ(1, bar.size());
			ASSERT_TRUE(bar.find(bar_id) != bar.end());

			auto num_of_requests = session.advanced().get_number_of_requests();

			auto foo = session.load<load_test::Foo>(bar.at(bar_id)->foo_id);

			ASSERT_FALSE(foo);
			ASSERT_EQ(num_of_requests, session.advanced().get_number_of_requests());
		}
	}

	TEST_F(LoadTest, PolymorphicLoad)
	{
		auto store = get_document_store(TEST_NAME);

		{
			auto session = store->open_session();

			auto cat = std::make_shared<load_test::Cat>();
			cat->set_name("Tom");

			auto dog = std::make_shared<load_test::Dog>();
			dog->set_name("Oscar");

			session.store(cat, "cats/1");
			session.store(dog, "dogs/1");
			session.save_changes();
		}
		{
			auto session = store->open_session();

			std::vector<std::string> ids = { "cats/1", "dogs/1" };
			auto have_names = session.load<load_test::IHaveName>(ids, load_test::IHaveName::from_json);

			ASSERT_EQ(2, have_names.size());

			ASSERT_TRUE(std::dynamic_pointer_cast<load_test::Dog>(have_names["dogs/1"]));
			ASSERT_EQ("Oscar", have_names["dogs/1"]->get_name());
			ASSERT_TRUE(std::dynamic_pointer_cast<load_test::Cat>(have_names["cats/1"]));
			ASSERT_EQ("Tom", have_names["cats/1"]->get_name());
		}
	}
}

