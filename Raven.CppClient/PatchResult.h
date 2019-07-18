#pragma once
#include "PatchStatus.h"
#include "DateTimeOffset.h"

namespace ravendb::client::documents::operations
{
	struct PatchResult
	{
		PatchStatus status = PatchStatus::UNSET;
		nlohmann::json::object_t modified_document{};
		nlohmann::json::object_t original_document{};
		nlohmann::json::object_t debug{};

		impl::DateTimeOffset last_modified{};
		std::string change_vector{};
		std::string collection{};
	};

	void from_json(const nlohmann::json& j, PatchResult& pr);
}
