#pragma once
#include "nlohmann/json.hpp"

namespace ravendb::client::documents::operations
{
	enum class PatchStatus
	{
		DOCUMENT_DOES_NOT_EXIST,
		CREATED,
		PATCHED,
		SKIPPED,
		NOT_MODIFIED,
		UNSET = -1
	};

	NLOHMANN_JSON_SERIALIZE_ENUM(PatchStatus,
		{
			{PatchStatus::UNSET, nullptr},
			{PatchStatus::DOCUMENT_DOES_NOT_EXIST, "DocumentDoesNotExists"},
			{PatchStatus::CREATED, "Created"},
			{PatchStatus::PATCHED, "Patched"},
			{PatchStatus::SKIPPED, "Skipped"},
			{PatchStatus::NOT_MODIFIED, "NotModified"}
		});
}
