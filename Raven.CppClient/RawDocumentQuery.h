#pragma once
#include "AbstractDocumentQuery.h"
#include "IRawDocumentQuery.h"

namespace ravendb::client::documents::session
{
	template<typename T>
	class RawDocumentQuery : 
		public AbstractDocumentQuery<T>,
		public IRawDocumentQuery<T, RawDocumentQuery<T>>
	{
	private:
		std::weak_ptr<RawDocumentQuery> _weak_this{};

		RawDocumentQuery(InMemoryDocumentSessionOperations& session, std::string raw_query)
			: AbstractDocumentQuery(session, {}, {}, false, {}, {}, {})
		{
			AbstractDocumentQuery<T>::query_raw = std::move(raw_query);
		}

	public:
		std::shared_ptr<DocumentConventions> get_conventions() const
		{
			return AbstractDocumentQuery<T>::get_conventions();
		}

		std::shared_ptr<RawDocumentQuery> skip(int32_t count)
		{
			AbstractDocumentQuery<T>::_skip(count);
			return _weak_this.lock();
		}

		std::shared_ptr<RawDocumentQuery> take(int32_t count)
		{
			AbstractDocumentQuery<T>::_take(count);
			return _weak_this.lock();
		}

		int32_t count()
		{
			return AbstractDocumentQuery<T>::count();
		}

		std::shared_ptr<RawDocumentQuery> no_tracking()
		{
			AbstractDocumentQuery<T>::_no_tracking();
			return _weak_this.lock();
		}

		std::shared_ptr<RawDocumentQuery> no_caching()
		{
			AbstractDocumentQuery<T>::_no_caching();
			return _weak_this.lock();
		}

		std::shared_ptr<RawDocumentQuery> using_default_operator(queries::QueryOperator query_operator)
		{
			AbstractDocumentQuery<T>::_using_default_operator(query_operator);
			return _weak_this.lock();
		}

		std::shared_ptr<RawDocumentQuery> add_parameter(std::string name, nlohmann::json value)
		{
			AbstractDocumentQuery<T>::_add_parameter(std::move(name), std::move(value));
			return _weak_this.lock();
		}

		std::vector<std::shared_ptr<T>> to_list()
		{
			return 	AbstractDocumentQuery<T>::to_list();
		}

		Lazy<std::vector<std::shared_ptr<T>>> lazily(
			const std::optional<std::function<void(const std::vector<std::shared_ptr<T>>&)>>& on_eval = {})
		{
			return AbstractDocumentQuery<T>::lazily(on_eval);
		}

		static std::shared_ptr<RawDocumentQuery> create(InMemoryDocumentSessionOperations& session, std::string raw_query)
		{
			auto new_object = std::shared_ptr<RawDocumentQuery>(new RawDocumentQuery(session, std::move(raw_query)));
			new_object->_weak_this = new_object;

			return new_object;
		}
	};
}
