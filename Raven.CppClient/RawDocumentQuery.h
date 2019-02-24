#pragma once
#include "AbstractDocumentQuery.h"
#include "QueryBase.h"

namespace ravendb::client::documents::session
{
	class RawDocumentQuery : public AbstractDocumentQuery, public IQueryBase<RawDocumentQuery>
	{
	private:
		std::weak_ptr<RawDocumentQuery> _weak_this{};

		RawDocumentQuery(InMemoryDocumentSessionOperations& session, std::string raw_query)
			: AbstractDocumentQuery(session, {}, {}, false, {}, {}, {})
		{
			query_raw = std::move(raw_query);
		}

	public:
		std::shared_ptr<RawDocumentQuery> skip(int32_t count)
		{
			_skip(count);
			return _weak_this.lock();
		}

		std::shared_ptr<RawDocumentQuery> take(int32_t count)
		{
			_take(count);
			return _weak_this.lock();
		}

		std::shared_ptr<RawDocumentQuery> no_tracking()
		{
			_no_tracking();
			return _weak_this.lock();
		}

		std::shared_ptr<RawDocumentQuery> no_caching()
		{
			_no_caching();
			return _weak_this.lock();
		}

		std::shared_ptr<RawDocumentQuery> using_default_operator(queries::QueryOperator query_operator)
		{
			_using_default_operator(query_operator);
			return _weak_this.lock();
		}

		std::shared_ptr<RawDocumentQuery> add_parameter(std::string name, nlohmann::json value)
		{
			_add_parameter(std::move(name), std::move(value));
			return _weak_this.lock();
		}

		static std::shared_ptr<RawDocumentQuery> create(InMemoryDocumentSessionOperations& session, std::string raw_query)
		{
			auto new_object = std::shared_ptr<RawDocumentQuery>(new RawDocumentQuery(session, std::move(raw_query)));
			new_object->_weak_this = new_object;

			return new_object;
		}
	};
}
