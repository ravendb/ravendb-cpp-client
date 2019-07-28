#include "pch.h"
#include "RavenTestDriver.h"
#include "raven_test_definitions.h"
#include "DocumentSession.h"
#include "EntityIdHelperUtil.h"
#include "User.h"
#include "AdvancedSessionOperations.h"
#include "ConcurrencyException.h"

namespace ravendb::client::tests::issues
{
	class RavenDB_13034Test : public driver::RavenTestDriver
	{
	protected:
		void customise_store(std::shared_ptr<client::documents::DocumentStore> store) override
		{
			//store->set_before_perform(infrastructure::set_for_fiddler);
		}

		static void SetUpTestCase()
		{
			REGISTER_ID_PROPERTY_FOR(infrastructure::entities::User, id);
		}
	};

	TEST_F(RavenDB_13034Test, ExploringConcurrencyBehavior)
	{
		auto store = get_document_store(TEST_NAME);

		{
			auto s1 = store->open_session();
			auto user = std::make_shared<infrastructure::entities::User>();
			user->name = "Nick";
			user->age = 99;
			s1.store(user, "users/1");
			s1.save_changes();
		}
		{
			auto s2 = store->open_session();
			s2.advanced().set_use_optimistic_concurrency(true);

			auto u2 = s2.load<infrastructure::entities::User>("users/1");

			{
				auto s3 = store->open_session();
				auto u3 = s3.load<infrastructure::entities::User>("users/1");
				ASSERT_NE(u2, u3);

				--u3->age;
				s3.save_changes();
			}
			++u2->age;

			auto u2_2 = s2.load<infrastructure::entities::User>("users/1");
			ASSERT_EQ(u2, u2_2);

			ASSERT_EQ(1, s2.advanced().get_number_of_requests());

			ASSERT_THROW(s2.save_changes(); ,exceptions::ConcurrencyException);

			ASSERT_EQ(2, s2.advanced().get_number_of_requests());

			auto u2_3 = s2.load<infrastructure::entities::User>("users/1");
			ASSERT_EQ(u2, u2_3);

			ASSERT_EQ(2, s2.advanced().get_number_of_requests());

			ASSERT_THROW(s2.save_changes();, exceptions::ConcurrencyException);
		}
		{
			auto s4 = store->open_session();
			auto u4 = s4.load<infrastructure::entities::User>("users/1");
			ASSERT_EQ(98, u4->age);
		}
	}
}
