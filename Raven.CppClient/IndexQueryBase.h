#pragma once
#include <ostream>
#include "json_utils.h"

namespace ravendb::client::documents::queries
{
	template<typename T>
	struct IndexQueryBase
	{
		virtual ~IndexQueryBase() = 0;

		std::optional<int32_t> page_size;
		std::string query;
		T query_parameters;
		int32_t start = 0;
		bool wait_for_non_stale_results = false;
		std::optional<std::chrono::milliseconds> wait_for_non_stale_results_timeout;

		friend std::ostream& operator<<(std::ostream& os, const IndexQueryBase& obj)
		{
			return os << obj.query;
		}
	};

	template <typename T>
	IndexQueryBase<T>::~IndexQueryBase() = default;

	template<typename T>
	void to_json(nlohmann::json& j, const IndexQueryBase<T>& iqb)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "PageSize", iqb.page_size, false);
		set_val_to_json(j, "Query", iqb.query);
		set_val_to_json(j, "QueryParameters", iqb.query_parameters);
		set_val_to_json(j, "Start", iqb.start);
		set_val_to_json(j, "WaitForNonStaleResults", iqb.wait_for_non_stale_results);
		set_val_to_json(j, "WaitForNonStaleResultsTimeout", iqb.wait_for_non_stale_results_timeout);
	}
}
