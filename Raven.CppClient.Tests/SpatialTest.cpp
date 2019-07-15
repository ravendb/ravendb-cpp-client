#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "AdvancedSessionOperations.h"
#include "AbstractIndexCreationTask.h"
#include "EntityIdHelperUtil.h"
#include "json_utils.h"

namespace ravendb::client::tests::client::spatial
{
	namespace spatial_test
	{
		struct MyDocumentItem
		{
			impl::DateTimeOffset date{};
			double latitude{};
			double longitude{};
		};

		void to_json(nlohmann::json& j, const MyDocumentItem& mdi)
		{
			using ravendb::client::impl::utils::json_utils::set_val_to_json;

			set_val_to_json(j, "latitude", mdi.latitude);
			set_val_to_json(j, "longitude", mdi.longitude);
			set_val_to_json(j, "date", mdi.date);
		}

		void from_json(const nlohmann::json& j, MyDocumentItem& mdi)
		{
			using ravendb::client::impl::utils::json_utils::get_val_from_json;

			get_val_from_json(j, "latitude", mdi.latitude);
			get_val_from_json(j, "longitude", mdi.longitude);
			get_val_from_json(j, "date", mdi.date);
		}

		struct MyDocument
		{
			std::string id{};
			std::vector<MyDocumentItem> items{};
		};

		void to_json(nlohmann::json& j, const MyDocument& md)
		{
			using ravendb::client::impl::utils::json_utils::set_val_to_json;

			set_val_to_json(j, "items", md.items);
		}

		void from_json(const nlohmann::json& j, MyDocument& md)
		{
			using ravendb::client::impl::utils::json_utils::get_val_from_json;

			get_val_from_json(j, "items", md.items);
		}

		struct MyProjection
		{
			std::string id{};
			impl::DateTimeOffset date{};
			double latitude{};
			double longitude{};
		};

		void to_json(nlohmann::json& j, const MyProjection& mp)
		{
			using ravendb::client::impl::utils::json_utils::set_val_to_json;

			set_val_to_json(j, "latitude", mp.latitude);
			set_val_to_json(j, "longitude", mp.longitude);
		}

		void from_json(const nlohmann::json& j, MyProjection& mp)
		{
			mp.latitude = j.at("latitude").is_string() ? std::stod(j.at("latitude").get<std::string>()) :
				j.at("latitude").get<decltype(mp.latitude)>();

			mp.longitude = j.at("longitude").is_string() ? std::stod(j.at("longitude").get<std::string>()) :
				j.at("longitude").get<decltype(mp.longitude)>();
		}
	}

	class SpatialTest : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<ravendb::client::documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}

		static void SetUpTestCase()
		{
			REGISTER_ID_PROPERTY_FOR(spatial_test::MyDocument, id);
			REGISTER_ID_PROPERTY_FOR(spatial_test::MyProjection, id);
		}

		class MyIndex : public documents::indexes::AbstractIndexCreationTask
		{
		public:
			~MyIndex() override = default;
			MyIndex()
			{
				SET_DEFAULT_INDEX_NAME();

				map = "docs.MyDocuments.SelectMany(doc => doc.items, (doc, item) => new {\n"
					"    doc = doc,\n"
					"    item = item\n"
					"}).Select(this0 => new {\n"
					"    this0 = this0,\n"
					"    lat = ((double)(this0.item.latitude ?? 0))\n"
					"}).Select(this1 => new {\n"
					"    this1 = this1,\n"
					"    lng = ((double)(this1.this0.item.longitude ?? 0))\n"
					"}).Select(this2 => new {\n"
					"    id = Id(this2.this1.this0.doc),\n"
					"    date = this2.this1.this0.item.date,\n"
					"    latitude = this2.this1.lat,\n"
					"    longitude = this2.lng,\n"
					"    coordinates = this.CreateSpatialField(((double ? ) this2.this1.lat), ((double ? ) this2.lng))\n"
					"})";

				store("id", documents::indexes::FieldStorage::YES);
				store("date", documents::indexes::FieldStorage::YES);

				store("latitude", documents::indexes::FieldStorage::YES);
				store("longitude", documents::indexes::FieldStorage::YES);
			}
		};
	};

	TEST_F(SpatialTest, WeirdSpatialResults)
	{
		auto store = get_document_store(TEST_NAME);

		{
			auto session = store->open_session();
			auto my_document = std::make_shared<spatial_test::MyDocument>();
			my_document->id = "First";

			auto my_document_item = spatial_test::MyDocumentItem();
			my_document_item.date = impl::DateTimeOffset("2019-07-08T00:00:00.0000000");
			my_document_item.latitude = 10.0;
			my_document_item.longitude = 10.0;

			my_document->items = { std::move(my_document_item) };

			session.store(my_document);
			session.save_changes();
		}

		MyIndex().execute(store);

		{
			auto session = store->open_session();
			auto stats_ref = std::shared_ptr<documents::session::QueryStatistics>();

			auto result = session.advanced().document_query<spatial_test::MyDocument, MyIndex>()
				->wait_for_non_stale_results()
				->within_radius_of("coordinates", 0, 12.3456789f, 12.3456789f)
				->statistics(stats_ref)
				->select_fields<spatial_test::MyProjection>({ "id", "latitude", "longitude" })
				->take(50)
				->to_list();

			ASSERT_EQ(0, stats_ref->total_results);

			ASSERT_TRUE(result.empty());
		}
	}

	TEST_F(SpatialTest, MatchSpatialResults)
	{
		auto store = get_document_store(TEST_NAME);

		{
			auto session = store->open_session();
			auto my_document = std::make_shared<spatial_test::MyDocument>();
			my_document->id = "First";

			auto my_document_item = spatial_test::MyDocumentItem();
			my_document_item.date = impl::DateTimeOffset("2019-07-08T00:00:00.0000000");
			my_document_item.latitude = 10.0;
			my_document_item.longitude = 10.0;

			my_document->items = { std::move(my_document_item) };

			session.store(my_document);
			session.save_changes();
		}

		MyIndex().execute(store);

		{
			auto session = store->open_session();
			auto stats_ref = std::shared_ptr<documents::session::QueryStatistics>();

			auto result = session.advanced().document_query<spatial_test::MyDocument, MyIndex>()
				->wait_for_non_stale_results()
				->within_radius_of("coordinates", 1, 10, 10)
				->statistics(stats_ref)
				->select_fields<spatial_test::MyProjection>({ "id", "latitude", "longitude" })
				->take(50)
				->to_list();

			ASSERT_EQ(1, stats_ref->total_results);

			ASSERT_EQ(1, result.size());
		}
	}
}
