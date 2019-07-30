#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "EntityIdHelperUtil.h"
#include "AbstractIndexCreationTask.h"
#include "IndexDoesNotExistException.h"

namespace ravendb::client::tests::issues
{
	namespace RavenDB_12790Test_ns
	{
		struct Document
		{
			std::string id{};
			std::string name{};
		};

		void from_json(const nlohmann::json& j, Document& d)
		{
			using ravendb::client::impl::utils::json_utils::get_val_from_json;

			get_val_from_json(j, "name", d.name);
		}

		void to_json(nlohmann::json& j, const Document& d)
		{
			using ravendb::client::impl::utils::json_utils::set_val_to_json;

			set_val_to_json(j, "name", d.name);
		}

		class DocumentIndex : public documents::indexes::AbstractIndexCreationTask
		{
		public:
			~DocumentIndex() override = default;
			DocumentIndex()
			{
				SET_DEFAULT_INDEX_NAME();
			}
		};
	}

	class RavenDB_12790Test : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<client::documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}

		static void SetUpTestCase()
		{
			REGISTER_ID_PROPERTY_FOR(RavenDB_12790Test_ns::Document, id);
		}
	};

	TEST_F(RavenDB_12790Test, LazyQueryAgainstMissingIndex)
	{
		auto store = get_document_store(TEST_NAME);

		{
			auto session = store->open_session();

			auto document = std::make_shared<RavenDB_12790Test_ns::Document>();
			document->name = "name";
			session.store(document);
			session.save_changes();
		}

		// intentionally not creating the index that we query against

		{
			bool success{ false };
			auto session = store->open_session();
			try
			{
				session.query<RavenDB_12790Test_ns::Document, RavenDB_12790Test_ns::DocumentIndex>()
					->to_list();
			}
			catch (const exceptions::documents::indexes::IndexDoesNotExistException&)
			{
				success = true;
			}
			ASSERT_TRUE(success);
		}
		{
			auto session = store->open_session();
			
			auto lazy_query = session.query<RavenDB_12790Test_ns::Document, RavenDB_12790Test_ns::DocumentIndex>()
				->lazily();

			ASSERT_THROW(lazy_query.get_value(); , exceptions::documents::indexes::IndexDoesNotExistException);
		}
	}
}
