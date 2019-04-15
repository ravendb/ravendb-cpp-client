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

		RawDocumentQuery(std::shared_ptr<InMemoryDocumentSessionOperations> session, std::string raw_query);

	public:
		static std::shared_ptr<IRawDocumentQuery<T, RawDocumentQuery>> create(
			std::shared_ptr<InMemoryDocumentSessionOperations> session, std::string raw_query);

		std::shared_ptr<DocumentConventions> get_conventions() const;


		std::shared_ptr<IRawDocumentQuery<T, RawDocumentQuery>> skip(int32_t count);

		std::shared_ptr<IRawDocumentQuery<T, RawDocumentQuery>> take(int32_t count);

		std::shared_ptr<IRawDocumentQuery<T, RawDocumentQuery>> wait_for_non_stale_results(
			const std::optional<std::chrono::milliseconds>& wait_timeout = {});

		std::shared_ptr<IRawDocumentQuery<T, RawDocumentQuery>> timings(
			std::optional<queries::timings::QueryTimings>& timings);

		std::shared_ptr<IRawDocumentQuery<T, RawDocumentQuery>> no_tracking();

		std::shared_ptr<IRawDocumentQuery<T, RawDocumentQuery>> no_caching();

		std::shared_ptr<IRawDocumentQuery<T, RawDocumentQuery>> using_default_operator(
			queries::QueryOperator query_operator);

		std::shared_ptr<IRawDocumentQuery<T, RawDocumentQuery>> statistics(std::optional<QueryStatistics>& stats) const;

		std::shared_ptr<IRawDocumentQuery<T, RawDocumentQuery>> wait_for_non_stale_results(
			const std::chrono::milliseconds& timeout = {});

		IndexQuery get_index_query() const;

		std::shared_ptr<IRawDocumentQuery<T, RawDocumentQuery>> add_after_query_executed_listener(
			std::function<void(const QueryResult&)> action);

		std::shared_ptr<IRawDocumentQuery<T, RawDocumentQuery>> remove_after_query_executed_listener(
			std::function<void(const QueryResult&)> action);

		std::shared_ptr<IRawDocumentQuery<T, RawDocumentQuery>> add_before_query_executed_listener(
			std::function<void(const IndexQuery&)> action);

		std::shared_ptr<IRawDocumentQuery<T, RawDocumentQuery>> remove_before_query_executed_listener(
			std::function<void(const IndexQuery&)> action);

		std::shared_ptr<IRawDocumentQuery<T, RawDocumentQuery>> add_after_stream_executed_listener(
			std::function<void(const nlohmann::json&)> action);

		std::shared_ptr<IRawDocumentQuery<T, RawDocumentQuery>> remove_after_stream_executed_listener(
			std::function<void(const nlohmann::json&)> action);

		std::shared_ptr<IRawDocumentQuery<T, RawDocumentQuery>> add_parameter(std::string name, nlohmann::json value);

		Lazy<int32_t> count_lazily();

		std::shared_ptr<T> first();

		std::shared_ptr<T> first_or_default();

		std::shared_ptr<T> single();

		bool any();

		int32_t count();

		Lazy<std::vector<std::shared_ptr<T>>> lazily(
			const std::optional<std::function<void(const std::vector<std::shared_ptr<T>>&)>>& on_eval = {});

		std::vector<std::shared_ptr<T>> to_list();
	};

	template <typename T>
	RawDocumentQuery<T>::RawDocumentQuery(std::shared_ptr<InMemoryDocumentSessionOperations> session,
		std::string raw_query)
		: AbstractDocumentQuery<T>(session, {}, {}, false, {}, {}, {})
	{
		AbstractDocumentQuery<T>::query_raw = std::move(raw_query);
	}

	template <typename T>
	std::shared_ptr<IRawDocumentQuery<T, RawDocumentQuery<T>>> RawDocumentQuery<T>::create(
		std::shared_ptr<InMemoryDocumentSessionOperations> session, std::string raw_query)
	{
		auto object = std::shared_ptr<RawDocumentQuery>(new RawDocumentQuery(session, std::move(raw_query)));
		object->_weak_this = object;

		return object;
	}

	template <typename T>
	std::shared_ptr<DocumentConventions> RawDocumentQuery<T>::get_conventions() const
	{
		return AbstractDocumentQuery<T>::get_conventions();
	}

	template <typename T>
	std::shared_ptr<IRawDocumentQuery<T, RawDocumentQuery<T>>> RawDocumentQuery<T>::skip(int32_t count)
	{
		AbstractDocumentQuery<T>::_skip(count);
		return _weak_this.lock();
	}

	template <typename T>
	std::shared_ptr<IRawDocumentQuery<T, RawDocumentQuery<T>>> RawDocumentQuery<T>::take(int32_t count)
	{
		AbstractDocumentQuery<T>::_take(count);
		return _weak_this.lock();
	}

	template <typename T>
	std::shared_ptr<IRawDocumentQuery<T, RawDocumentQuery<T>>> RawDocumentQuery<T>::wait_for_non_stale_results(
		const std::optional<std::chrono::milliseconds>& wait_timeout)
	{
		AbstractDocumentQuery<T>::_wait_for_non_stale_results(wait_timeout);
		return _weak_this.lock();
	}

	template <typename T>
	std::shared_ptr<IRawDocumentQuery<T, RawDocumentQuery<T>>> RawDocumentQuery<T>::timings(
		std::optional<queries::timings::QueryTimings>& timings)
	{
		AbstractDocumentQuery<T>::_wait_for_non_stale_results(timings);
		return _weak_this.lock();
	}

	template <typename T>
	std::shared_ptr<IRawDocumentQuery<T, RawDocumentQuery<T>>> RawDocumentQuery<T>::no_tracking()
	{
		AbstractDocumentQuery<T>::_no_tracking();
		return _weak_this.lock();
	}

	template <typename T>
	std::shared_ptr<IRawDocumentQuery<T, RawDocumentQuery<T>>> RawDocumentQuery<T>::no_caching()
	{
		AbstractDocumentQuery<T>::_no_caching();
		return _weak_this.lock();
	}

	template <typename T>
	std::shared_ptr<IRawDocumentQuery<T, RawDocumentQuery<T>>> RawDocumentQuery<T>::using_default_operator(
		queries::QueryOperator query_operator)
	{
		AbstractDocumentQuery<T>::_using_default_operator(query_operator);
		return _weak_this.lock();
	}

	template <typename T>
	std::shared_ptr<IRawDocumentQuery<T, RawDocumentQuery<T>>> RawDocumentQuery<T>::statistics(
		std::optional<QueryStatistics>& stats) const
	{
		AbstractDocumentQuery<T>::_statistics(stats);
		return _weak_this.lock();
	}

	template <typename T>
	std::shared_ptr<IRawDocumentQuery<T, RawDocumentQuery<T>>> RawDocumentQuery<T>::wait_for_non_stale_results(
		const std::chrono::milliseconds& timeout)
	{
		AbstractDocumentQuery<T>::_wait_for_non_stale_results(timeout);
		return _weak_this.lock();
	}

	template <typename T>
	IndexQuery RawDocumentQuery<T>::get_index_query() const
	{
		return AbstractDocumentQuery<T>::get_index_query();
	}

	template <typename T>
	std::shared_ptr<IRawDocumentQuery<T, RawDocumentQuery<T>>> RawDocumentQuery<T>::
		remove_after_query_executed_listener(std::function<void(const QueryResult&)> action)
	{
		AbstractDocumentQuery<T>::_remove_after_query_executed_listener(action);
		return _weak_this.lock();
	}

	template <typename T>
	std::shared_ptr<IRawDocumentQuery<T, RawDocumentQuery<T>>> RawDocumentQuery<T>::add_after_query_executed_listener(
		std::function<void(const QueryResult&)> action)
	{
		AbstractDocumentQuery<T>::_add_after_query_executed_listener(action);
		return _weak_this.lock();
	}

	template <typename T>
	std::shared_ptr<IRawDocumentQuery<T, RawDocumentQuery<T>>> RawDocumentQuery<T>::add_before_query_executed_listener(
		std::function<void(const IndexQuery&)> action)
	{
		AbstractDocumentQuery<T>::_add_before_query_executed_listener(action);
		return _weak_this.lock();
	}

	template <typename T>
	std::shared_ptr<IRawDocumentQuery<T, RawDocumentQuery<T>>> RawDocumentQuery<T>::
		remove_before_query_executed_listener(std::function<void(const IndexQuery&)> action)
	{
		AbstractDocumentQuery<T>::_remove_before_query_executed_listener(action);
		return _weak_this.lock();
	}

	template <typename T>
	std::shared_ptr<IRawDocumentQuery<T, RawDocumentQuery<T>>> RawDocumentQuery<T>::add_after_stream_executed_listener(
		std::function<void(const nlohmann::json&)> action)
	{
		AbstractDocumentQuery<T>::_add_after_stream_executed_listener(action);
		return _weak_this.lock();
	}

	template <typename T>
	std::shared_ptr<IRawDocumentQuery<T, RawDocumentQuery<T>>> RawDocumentQuery<T>::
		remove_after_stream_executed_listener(std::function<void(const nlohmann::json&)> action)
	{
		AbstractDocumentQuery<T>::_remove_after_stream_executed_listener(action);
		return _weak_this.lock();
	}

	template <typename T>
	std::shared_ptr<IRawDocumentQuery<T, RawDocumentQuery<T>>> RawDocumentQuery<T>::add_parameter(
		std::string name, nlohmann::json value)
	{
		AbstractDocumentQuery<T>::_add_parameter(std::move(name), std::move(value));
		return _weak_this.lock();
	}

	template <typename T>
	Lazy<int32_t> RawDocumentQuery<T>::count_lazily()
	{
		return AbstractDocumentQuery<T>::count_lazily();
	}

	template <typename T>
	std::shared_ptr<T> RawDocumentQuery<T>::first()
	{
		return AbstractDocumentQuery<T>::first();
	}

	template <typename T>
	std::shared_ptr<T> RawDocumentQuery<T>::first_or_default()
	{
		return AbstractDocumentQuery<T>::first_or_default();
	}

	template <typename T>
	std::shared_ptr<T> RawDocumentQuery<T>::single()
	{
		return AbstractDocumentQuery<T>::single();
	}

	template <typename T>
	bool RawDocumentQuery<T>::any()
	{
		return AbstractDocumentQuery<T>::any();
	}

	template <typename T>
	int32_t RawDocumentQuery<T>::count()
	{
		return AbstractDocumentQuery<T>::count();
	}

	template <typename T>
	Lazy<std::vector<std::shared_ptr<T>>> RawDocumentQuery<T>::lazily(
		const std::optional<std::function<void(const std::vector<std::shared_ptr<T>>&)>>& on_eval)
	{
		return AbstractDocumentQuery<T>::lazily(on_eval);
	}

	template <typename T>
	std::vector<std::shared_ptr<T>> RawDocumentQuery<T>::to_list()
	{
		return AbstractDocumentQuery<T>::to_list();
	}
}
