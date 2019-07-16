#pragma once
#include "DateTimeOffset.h"
#include "json.hpp"

namespace ravendb::client::documents::indexes
{
	struct IndexingError
	{
		std::string error{};
		ravendb::client::impl::DateTimeOffset time_stamp{};
		std::string document{};
		std::string action{};

		std::string to_string() const;
	};

	void to_json(nlohmann::json& j, const IndexingError& ie);

	void from_json(const nlohmann::json& j, IndexingError& ie);
}
