#pragma once
#include "AbstractDocumentQuery.h"
#include "QueryBase.h"

namespace ravendb::client::documents::session
{
	class RawDocumentQuery final : public AbstractDocumentQuery, public QueryBase<RawDocumentQuery>
	{
	private:
		class InnerImpl final : public AbstractInner
		{
		public:
			~InnerImpl() override = default;

			void inner_skip(int32_t count) override
			{
				_query_base_ptr.lock()->_skip(count);
			}

			void inner_take(int32_t count) override
			{
				_query_base_ptr.lock()->_take(count);
			}

			void inner_no_tracking() override
			{
				_query_base_ptr.lock()->_no_tracking();
			}

			void inner_no_caching() override
			{
				_query_base_ptr.lock()->_no_caching();
			}

			void inner_using_default_operator(queries::QueryOperator query_operator) override
			{
				_query_base_ptr.lock()->_using_default_operator(query_operator);
			}

			void inner_add_parameter(std::string name, nlohmann::json value) override
			{
				_query_base_ptr.lock()->_add_parameter(std::move(name), std::move(value));
			}
		};

		RawDocumentQuery(InMemoryDocumentSessionOperations& session, std::string raw_query)
			: AbstractDocumentQuery(session, {}, {}, false, {}, {}, {})
			, QueryBase(std::make_unique<InnerImpl>())
		{
			query_raw = std::move(raw_query);
		}

	public:	
		static std::shared_ptr<RawDocumentQuery> create(InMemoryDocumentSessionOperations& session, std::string raw_query)
		{
			auto new_object = std::shared_ptr<RawDocumentQuery>(new RawDocumentQuery(session, std::move(raw_query)));
			new_object->set_this_ptr(new_object);

			return new_object;
		}
	};
}
