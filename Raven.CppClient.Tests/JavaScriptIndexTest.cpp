#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "User.h"
#include "EntityIdHelperUtil.h"
#include "DocumentSession.h"
#include "AbstractJavaScriptIndexCreationTask.h"

//TODO add more asserts according to java client fixes

namespace javascript_index_test
{
	struct Category
	{
		std::string description{};
		std::string name{};
	};

	void to_json(nlohmann::json& j, const Category& c)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "description", c.description);
		set_val_to_json(j, "name", c.name);
	}

	void from_json(const nlohmann::json& j, Category& c)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "description", c.description);
		get_val_from_json(j, "name", c.name);
	}

	struct Product2
	{
		std::string category{};
		std::string name{};
		int32_t price_per_unit{};

		Product2(std::string category_param, std::string name_param, int32_t price_per_unit_param)
			: category(std::move(category_param))
			, name(std::move(name_param))
			, price_per_unit(price_per_unit_param)
		{}
	};

	void to_json(nlohmann::json& j, const Product2& p)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "category", p.category);
		set_val_to_json(j, "name", p.name);
		set_val_to_json(j, "price_per_unit", p.price_per_unit);
	}

	void from_json(const nlohmann::json& j, Product2& p)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "category", p.category);
		get_val_from_json(j, "name", p.name);
		get_val_from_json(j, "pricePerUnit", p.price_per_unit);
	}

	struct CategoryCount
	{
		std::string category{};
		int32_t count{};
	};

	void to_json(nlohmann::json& j, const CategoryCount& c)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "category", c.category);
		set_val_to_json(j, "count", c.count);
	}

	void from_json(const nlohmann::json& j, CategoryCount& c)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "category", c.category);
		get_val_from_json(j, "count", c.count);
	}

	struct Product
	{
		std::string name{};
		bool available{};
	};

	void to_json(nlohmann::json& j, const Product& p)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "name", p.name);
		set_val_to_json(j, "available", p.available);
	}

	void from_json(const nlohmann::json& j, Product& p)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "name", p.name);
		get_val_from_json(j, "available", p.available);
	}

	class UsersByName : public ravendb::client::documents::indexes::AbstractJavaScriptIndexCreationTask
	{
	public:
		~UsersByName() override = default;
		UsersByName()
		{
			SET_DEFAULT_INDEX_NAME;

			set_maps({ "map('Users', function(u) { return { name: u.name, count : 1 } })" });
		}
	};

	class UsersByNameWithAdditionalSources : public ravendb::client::documents::indexes::AbstractJavaScriptIndexCreationTask
	{
	public:
		~UsersByNameWithAdditionalSources() override = default;
		UsersByNameWithAdditionalSources()
		{
			SET_DEFAULT_INDEX_NAME;

			set_maps({ "map('Users', function(u) { return { name: mr(u.name)}; })" });
			std::unordered_map<std::string, std::string> additional_sources{};
			additional_sources.insert_or_assign("The Script", "function mr(x) { return 'Mr. ' + x; }");
			set_additional_sources(additional_sources);
		}
	};

	class FanoutByNumbersWithReduce : public ravendb::client::documents::indexes::AbstractJavaScriptIndexCreationTask
	{
	public:
		~FanoutByNumbersWithReduce() override = default;
		FanoutByNumbersWithReduce()
		{
			SET_DEFAULT_INDEX_NAME;

			std::string map = R"(
				map('Fanouts', function (f)
				{
					var result = [];
                    for(var i = 0; i < f.numbers.length; i++)
                    {
						result.push(
						{
							foo: f.foo,
							sum: f.numbers[i]
						});
					}
					return result;
				}))";
			set_maps({ map });
			set_reduce("groupBy(f => f.foo).aggregate(g => ({  foo: g.key, sum: g.values.reduce((total, val) => val.sum + total,0) }))");
		}

		struct Result
		{
			std::string foo{};
			int32_t sum{};
		};
	};

	void to_json(nlohmann::json& j, const FanoutByNumbersWithReduce::Result& r)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "foo", r.foo);
		set_val_to_json(j, "sum", r.sum);
	}

	void from_json(const nlohmann::json& j, FanoutByNumbersWithReduce::Result& r)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "foo", r.foo);
		get_val_from_json(j, "sum", r.sum);
	}

	class UsersByNameAndAnalyzedName : public ravendb::client::documents::indexes::AbstractJavaScriptIndexCreationTask
	{
	public:
		~UsersByNameAndAnalyzedName() override = default;
		UsersByNameAndAnalyzedName()
		{
			SET_DEFAULT_INDEX_NAME;

			std::string map = R"(
				map('Users', function (u){
					return {
							name: u.name,
							_: {$value: u.name, $name:'analyzedName', $options: { indexing: 'Search', storage: true}}
					};
				}))";
			set_maps({ map });

			auto index_field_options = ravendb::client::documents::indexes::IndexFieldOptions();
			index_field_options.indexing = ravendb::client::documents::indexes::FieldIndexing::SEARCH;
			index_field_options.analyzer = "StandardAnalyzer";

			std::unordered_map<std::string, ravendb::client::documents::indexes::IndexFieldOptions> field_options{};
			field_options.insert_or_assign(ravendb::client::constants::documents::indexing::fields::ALL_FIELDS,
				std::move(index_field_options));
			set_fields(std::move(field_options));
		}

		struct Result
		{
			std::string analyzed_name{};
		};
	};

	void to_json(nlohmann::json& j, const UsersByNameAndAnalyzedName::Result& r)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "analyzedName", r.analyzed_name);
	}

	void from_json(const nlohmann::json& j, UsersByNameAndAnalyzedName::Result& r)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "analyzedName", r.analyzed_name);
	}

	class UsersAndProductsByName : public ravendb::client::documents::indexes::AbstractJavaScriptIndexCreationTask
	{
	public:
		~UsersAndProductsByName() override = default;
		UsersAndProductsByName()
		{
			SET_DEFAULT_INDEX_NAME;

			set_maps({ "map('Users', function (u){ return { name: u.name, count: 1};})", "map('Products', function (p){ return { name: p.name, count: 1};})" });
		}
	};

	class UsersAndProductsByNameAndCount : public ravendb::client::documents::indexes::AbstractJavaScriptIndexCreationTask
	{
	public:
		~UsersAndProductsByNameAndCount() override = default;
		UsersAndProductsByNameAndCount()
		{
			SET_DEFAULT_INDEX_NAME;

			set_maps({ "map('Users', function (u){ return { name: u.name, count: 1};})", "map('Products', function (p){ return { name: p.name, count: 1};})" });
			set_reduce(R"(
				groupBy( x =>  x.name )
					.aggregate(g => {return {
						name: g.key,
						count: g.values.reduce((total, val) => val.count + total,0)
				};}))");
		}
	};

	class Products_ByCategory : public ravendb::client::documents::indexes::AbstractJavaScriptIndexCreationTask
	{
	public:
		~Products_ByCategory() override = default;
		Products_ByCategory()
		{
			SET_DEFAULT_INDEX_NAME;

			std::string map = R"(
				map('products', function(p){
                    return {
						category: load(p.category, 'Categories').name,
						count: 1
					}
				}))";
			set_maps({ map });

			set_reduce(R"(
				groupBy( x => x.category )
				.aggregate(g => {
					return {
						category: g.key,
						count: g.values.reduce((count, val) => val.count + count, 0)
					};
				}))");

			set_output_reduce_to_collection("CategoryCounts");
		}

		struct Result
		{
			std::string category{};
			int32_t count{};
		};
	};

	void to_json(nlohmann::json& j, const Products_ByCategory::Result& r)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "category", r.category);
		set_val_to_json(j, "count", r.count);
	}

	void from_json(const nlohmann::json& j, Products_ByCategory::Result& r)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "category", r.category);
		get_val_from_json(j, "count", r.count);
	}

	struct Fanout
	{
		std::string foo{};
		std::vector<int32_t> numbers{};
	};

	void to_json(nlohmann::json& j, const Fanout& f)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "foo", f.foo);
		set_val_to_json(j, "numbers", f.numbers);
	}

	void from_json(const nlohmann::json& j, Fanout& f)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "foo", f.foo);
		get_val_from_json(j, "numbers", f.numbers);
	}
}

namespace ravendb::client::tests::client::indexing
{
	class JavaScriptIndexTest : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<ravendb::client::documents::DocumentStore> store) override
		{
			store->set_before_perform(infrastructure::set_for_fiddler);
		}

		static void SetUpTestCase()
		{
			REGISTER_ID_PROPERTY_FOR(ravendb::client::tests::infrastructure::entities::User, id);
		}	
	};

	TEST_F(JavaScriptIndexTest, CanCreateIndexesUsingIndexCreation)
	{
		auto store = get_document_store(TEST_NAME);
		store->execute_index(std::make_shared<javascript_index_test::UsersByName>());
		{
			auto session = store->open_session();
			auto user = std::make_shared<infrastructure::entities::User>();
			user->name = "Brendan Eich";
			session.store(user);
			session.save_changes();
		}

		wait_for_indexing(store);

		{
			auto session = store->open_session();
			auto single = session.query<infrastructure::entities::User>(documents::queries::Query::index("UsersByName"))
				->where_equals("name", "Brendan Eich")
				->single();

			ASSERT_TRUE(single);
		}
	}

	TEST_F(JavaScriptIndexTest, CanUseJavaScriptIndexWithAdditionalSources)
	{
		auto store = get_document_store(TEST_NAME);
		store->execute_index(std::make_shared<javascript_index_test::UsersByNameWithAdditionalSources>());

		auto session = store->open_session();
		auto user = std::make_shared<infrastructure::entities::User>();
		user->name = "Brendan Eich";
		session.store(user);
		session.save_changes();

		wait_for_indexing(store);

		auto res = session.query<infrastructure::entities::User>(documents::queries::Query::index("UsersByNameWithAdditionalSources"))
			->where_equals("name", "Mr. Brendan Eich")
			->single();

		ASSERT_TRUE(res);
	}

	TEST_F(JavaScriptIndexTest, CanIndexMapReduceWithFanout)
	{
		auto store = get_document_store(TEST_NAME);
		store->execute_index(std::make_shared<javascript_index_test::FanoutByNumbersWithReduce>());

		auto session = store->open_session();

		auto fanout1 = std::make_shared<javascript_index_test::Fanout>();
		fanout1->foo = "Foo";
		fanout1->numbers = { 4, 6, 11, 9 };

		auto fanout2 = std::make_shared<javascript_index_test::Fanout>();
		fanout1->foo = "Bar";
		fanout1->numbers = { 3, 8, 5, 17 };

		session.store(fanout1);
		session.store(fanout2);
		session.save_changes();

		wait_for_indexing(store);

		auto res = session.query<javascript_index_test::FanoutByNumbersWithReduce::Result>(
				documents::queries::Query::index("FanoutByNumbersWithReduce"))
			->where_equals("sum", 33)
			->single();
	}


	TEST_F(JavaScriptIndexTest, CanUseJavaScriptIndexWithDynamicFields)
	{
		auto store = get_document_store(TEST_NAME);
		store->execute_index(std::make_shared<javascript_index_test::UsersByNameAndAnalyzedName>());

		auto session = store->open_session();

		auto user = std::make_shared<infrastructure::entities::User>();
		user->name = "Brendan Eich";
		session.store(user);
		session.save_changes();

		wait_for_indexing(store);

		auto res = session.query<infrastructure::entities::User>(documents::queries::Query::index("UsersByNameAndAnalyzedName"))
			->select_fields<javascript_index_test::UsersByNameAndAnalyzedName::Result>()
			->search("analyzedName", "Brendan")
			->single();
	}

	TEST_F(JavaScriptIndexTest, CanUseJavaScriptMultiMapIndex)
	{
		auto store = get_document_store(TEST_NAME);
		store->execute_index(std::make_shared<javascript_index_test::UsersAndProductsByName>());

		auto session = store->open_session();

		auto user = std::make_shared<infrastructure::entities::User>();
		user->name = "Brendan Eich";
		session.store(user);

		auto product = std::make_shared<javascript_index_test::Product>();
		product->name = "Shampoo";
		product->available = true;
		session.store(product);

		session.save_changes();

		wait_for_indexing(store);

		auto res = session.query<infrastructure::entities::User>(documents::queries::Query::index("UsersAndProductsByName"))
			->where_equals("name", "Brendan Eich")
			->single();
	}

	TEST_F(JavaScriptIndexTest, CanUseJavaScriptMapReduceIndex)
	{
		auto store = get_document_store(TEST_NAME);
		store->execute_index(std::make_shared<javascript_index_test::UsersAndProductsByNameAndCount>());

		auto session = store->open_session();

		auto user = std::make_shared<infrastructure::entities::User>();
		user->name = "Brendan Eich";
		session.store(user);

		auto product = std::make_shared<javascript_index_test::Product>();
		product->name = "Shampoo";
		product->available = true;
		session.store(product);

		session.save_changes();

		wait_for_indexing(store);

		auto res = session.query<infrastructure::entities::User>(documents::queries::Query::index("UsersAndProductsByNameAndCount"))
			->where_equals("name", "Brendan Eich")
			->single();
	}

	TEST_F(JavaScriptIndexTest, OutputReduceToCollection)
	{
		auto store = get_document_store(TEST_NAME);
		store->execute_index(std::make_shared<javascript_index_test::Products_ByCategory>());

		auto session = store->open_session();

		auto category1 = std::make_shared<javascript_index_test::Category>();
		category1->name = "Beverages";
		session.store(category1, "categories/1-A");

		auto category2 = std::make_shared<javascript_index_test::Category>();
		category1->name = "Seafood";
		session.store(category1, "categories/2-A");

		session.store(std::make_shared<javascript_index_test::Product2>("categories/1-A", "Lakkalikoori", 13));
		session.store(std::make_shared<javascript_index_test::Product2>("categories/1-A", "Original Frankfurter", 16));
		session.store(std::make_shared<javascript_index_test::Product2>("categories/2-A", "Rod Kaviar", 18));

		session.save_changes();

		wait_for_indexing(store);

		auto res = session.query<javascript_index_test::Products_ByCategory::Result>(documents::queries::Query::index("Products/ByCategory"))
			->to_list();

		auto res2 = session.query<javascript_index_test::CategoryCount>()
			->to_list();

		ASSERT_EQ(res.size(), res2.size());
	}
}
