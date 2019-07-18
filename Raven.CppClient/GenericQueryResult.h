#pragma once
#include "QueryResultBase.h"

namespace ravendb::client::documents::queries
{
	template<typename TResult, typename TInclude>
	struct GenericQueryResult : QueryResultBase<TResult, TInclude>
	{
		~GenericQueryResult() override = 0;

		int32_t total_results{};
		int32_t skipped_results{};
		std::optional<std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>>> highlightings{};
		std::optional<std::unordered_map<std::string, std::vector<std::string>>> explanations{};
		int64_t duration_in_ms{};
		std::optional<std::unordered_map<std::string, std::string>> score_explanations{};
	};

	template <typename TResult, typename TInclude>
	GenericQueryResult<TResult, TInclude>::~GenericQueryResult() = default;

	template<typename TResult, typename TInclude>
	void from_json(const nlohmann::json& j, GenericQueryResult<TResult, TInclude>& gqr)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		from_json(j, static_cast<QueryResultBase<TResult, TInclude>&>(gqr));

		get_val_from_json(j, "TotalResults", gqr.total_results);
		get_val_from_json(j, "SkippedResults", gqr.skipped_results);
		get_val_from_json(j, "Highlightings", gqr.highlightings);
		get_val_from_json(j, "Explanations", gqr.explanations);
		get_val_from_json(j, "DurationInMs", gqr.duration_in_ms);
		get_val_from_json(j, "ScoreExplanations", gqr.score_explanations);
	}
}
