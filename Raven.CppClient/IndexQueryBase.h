#pragma once
#include <ostream>

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
		std::chrono::milliseconds wait_for_non_stale_results_timeout;

		friend std::ostream& operator<<(std::ostream& os, const IndexQueryBase& obj)
		{
			return os << obj.query;
		}
	};

	template <typename T>
	IndexQueryBase<T>::~IndexQueryBase() = default;
}
