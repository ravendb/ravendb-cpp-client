#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "PointField.h"

namespace ravendb::client::tests::client::spatial
{
	namespace ravendb_9676_test
	{
		struct Item
		{
			std::string name{};
			double latitude{};
			double longitude{};
		};

		void to_json(nlohmann::json& j, const Item& i)
		{
			using ravendb::client::impl::utils::json_utils::set_val_to_json;

			set_val_to_json(j, "name", i.name);
			set_val_to_json(j, "latitude", i.latitude);
			set_val_to_json(j, "longitude", i.longitude);
		}

		void from_json(const nlohmann::json& j, Item& i)
		{
			using ravendb::client::impl::utils::json_utils::get_val_from_json;

			get_val_from_json(j, "name", i.name);
			get_val_from_json(j, "latitude", i.latitude);
			get_val_from_json(j, "longitude", i.longitude);
		}
	}

	class RavenDB_9676Test : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<ravendb::client::documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}
	};

	TEST_F(RavenDB_9676Test, CanOrderByDistanceOnDynamicSpatialField)
	{
		auto store = get_document_store(TEST_NAME);
		{
			auto session = store->open_session();

			auto item = std::make_shared<ravendb_9676_test::Item>();
			item->name = "Item1";
			item->latitude = 10;
			item->longitude = 10;
			session.store(item);

			auto item1 = std::make_shared<ravendb_9676_test::Item>();
			item1->name = "Item2";
			item1->latitude = 11;
			item1->longitude = 11;
			session.store(item1);

			session.save_changes();
		}
		{
			auto session = store->open_session();
			auto items = session.query<ravendb_9676_test::Item>()
				->wait_for_non_stale_results()
				->spatial(documents::queries::spatial::PointField("latitude", "longitude"),
					[](auto f) {return f.within_radius(1000, 10, 10); })
				->order_by_distance(documents::queries::spatial::PointField("latitude", "longitude"),
					10, 10)
				->to_list();

			ASSERT_EQ(2, items.size());
			ASSERT_EQ("Item1", items.at(0)->name);
			ASSERT_EQ("Item2", items.at(1)->name);

			auto items2 = session.query<ravendb_9676_test::Item>()
				->wait_for_non_stale_results()
				->spatial(documents::queries::spatial::PointField("latitude", "longitude"),
					[](auto f) {return f.within_radius(1000, 10, 10); })
				->order_by_distance_descending(documents::queries::spatial::PointField("latitude", "longitude"),
					10, 10)
				->to_list();

			ASSERT_EQ(2, items2.size());
			ASSERT_EQ("Item2", items2.at(0)->name);
			ASSERT_EQ("Item1", items2.at(1)->name);
		}
	}
}
