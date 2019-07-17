#pragma once
#include "json.hpp"

namespace ravendb::client::documents::operations
{
	struct OperationExceptionResult
	{
		std::string type{};
		std::string message{};
		std::string error;
		int32_t status_code{};
	};

	void from_json(const nlohmann::json& j, OperationExceptionResult& oer);
}
