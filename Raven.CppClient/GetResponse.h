#pragma once
#include "CompareStringsLessThanIgnoreCase.h"
#include "nlohmann/json.hpp"

namespace ravendb::client::documents::commands::multi_get
{
	struct GetResponse
	{
		std::optional<std::string> result{};
		std::map<std::string, std::string, impl::utils::CompareStringsLessThanIgnoreCase> headers{};
		int32_t status_code{};
		bool force_retry{ false };

		bool request_has_errors() const;
	};

	void from_json(const nlohmann::json& j, GetResponse& gr);
}
