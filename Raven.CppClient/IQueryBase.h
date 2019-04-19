#pragma once

namespace ravendb::client::documents::queries
{
	enum class QueryOperator;
}

namespace ravendb::client::documents::session
{
	template<typename T, class TThis>
	class IQueryBase
	{
	private:
		TThis* cast_down()
		{
			static_assert(std::is_base_of_v<IQueryBase, TThis>,
				"'TThis' should be derived from IQueryBase");
			return static_cast<TThis*>(this);
		}

	protected:
		IQueryBase() = default;

	public:
		std::shared_ptr<DocumentConventions> get_conventions() const;

		auto add_before_query_executed_listener(std::function<void(const IndexQuery&)> action);
		auto remove_before_query_executed_listener(std::function<void(const IndexQuery&)> action);

		auto add_after_query_executed_listener(std::function<void(const QueryResult&)> action);
		auto remove_after_query_executed_listener(std::function<void(const QueryResult&)> action);

		auto add_after_stream_executed_listener(std::function<void(const nlohmann::json&)> action);
		auto remove_after_stream_executed_listener(std::function<void(const nlohmann::json&)> action);

		void invoke_after_query_executed(const QueryResult& result);
		void invoke_after_stream_executed(const nlohmann::json& result);

		auto no_caching();

		auto no_tracking();

		auto timings(std::shared_ptr<queries::timings::QueryTimings>& timings_reference);

		auto skip(int32_t count);

		auto statistics(std::shared_ptr<QueryStatistics>& stats_reference);

		auto take(int32_t count);

		auto using_default_operator(queries::QueryOperator query_operator);

		auto wait_for_non_stale_results(const std::optional<std::chrono::milliseconds>& timeout = {});

		IndexQuery get_index_query() const;

		auto add_parameter(std::string name, nlohmann::json value);
	};

	template <typename T, class TThis>
	std::shared_ptr<DocumentConventions> IQueryBase<T, TThis>::get_conventions() const
	{
		return cast_down()->get_conventions();
	}

	template <typename T, class TThis>
	auto IQueryBase<T, TThis>::add_before_query_executed_listener(
		std::function<void(const IndexQuery&)> action)
	{
		return cast_down()->add_before_query_executed_listener(action);
	}

	template <typename T, class TThis>
	auto IQueryBase<T, TThis>::remove_before_query_executed_listener(
		std::function<void(const IndexQuery&)> action)
	{
		return cast_down()->remove_before_query_executed_listener(action);
	}

	template <typename T, class TThis>
	auto IQueryBase<T, TThis>::add_after_query_executed_listener(
		std::function<void(const QueryResult&)> action)
	{
		return cast_down()->add_after_query_executed_listener(action);
	}

	template <typename T, class TThis>
	auto IQueryBase<T, TThis>::remove_after_query_executed_listener(
		std::function<void(const QueryResult&)> action)
	{
		return cast_down()->remove_after_query_executed_listener(action);
	}

	template <typename T, class TThis>
	auto IQueryBase<T, TThis>::add_after_stream_executed_listener(
		std::function<void(const nlohmann::json&)> action)
	{
		return cast_down()->add_after_stream_executed_listener(action);
	}

	template <typename T, class TThis>
	auto IQueryBase<T, TThis>::remove_after_stream_executed_listener(
		std::function<void(const nlohmann::json&)> action)
	{
		return cast_down()->remove_after_stream_executed_listener(action);
	}

	template <typename T, class TThis>
	void IQueryBase<T, TThis>::invoke_after_query_executed(const QueryResult& result)
	{
		return cast_down()->invoke_after_query_executed(result);
	}

	template <typename T, class TThis>
	void IQueryBase<T, TThis>::invoke_after_stream_executed(const nlohmann::json& result)
	{
		return cast_down()->invoke_after_stream_executed(result);
	}

	template <typename T, class TThis>
	auto IQueryBase<T, TThis>::no_caching()
	{
		return cast_down()->no_caching();
	}

	template <typename T, class TThis>
	auto IQueryBase<T, TThis>::no_tracking()
	{
		return cast_down()->no_tracking();
	}

	template <typename T, class TThis>
	auto IQueryBase<T, TThis>::timings(std::shared_ptr<queries::timings::QueryTimings>& timings_reference)
	{
		return cast_down()->timings(timings_reference);
	}

	template <typename T, class TThis>
	auto IQueryBase<T, TThis>::skip(int32_t count)
	{
		return cast_down()->skip(count);
	}

	template <typename T, class TThis>
	auto IQueryBase<T, TThis>::statistics(std::shared_ptr<QueryStatistics>& stats_reference)
	{
		return cast_down()->statistics(stats_reference);
	}

	template <typename T, class TThis>
	auto IQueryBase<T, TThis>::take(int32_t count)
	{
		return cast_down()->take(count);
	}

	template <typename T, class TThis>
	auto IQueryBase<T, TThis>::using_default_operator(queries::QueryOperator query_operator)
	{
		return cast_down()->using_default_operator(query_operator);
	}

	template <typename T, class TThis>
	auto IQueryBase<T, TThis>::wait_for_non_stale_results(const std::optional<std::chrono::milliseconds>& timeout)
	{
		return cast_down()->wait_for_non_stale_results(timeout);
	}

	template <typename T, class TThis>
	IndexQuery IQueryBase<T, TThis>::get_index_query() const
	{
		return cast_down()->get_index_query();
	}

	template <typename T, class TThis>
	auto IQueryBase<T, TThis>::add_parameter(std::string name, nlohmann::json value)
	{
		return cast_down()->add_parameter(std::move(name), std::move(value));
	}
}
