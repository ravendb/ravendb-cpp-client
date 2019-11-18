#pragma once
#include "nlohmann/json.hpp"


namespace ravendb::client::documents::indexes
{
	enum class IndexRunningStatus
	{
		RUNNING,
		PAUSED,
		DISABLED,
		UNSET = -1
	};

	NLOHMANN_JSON_SERIALIZE_ENUM(IndexRunningStatus,
		{
			{IndexRunningStatus::UNSET, nullptr},
			{IndexRunningStatus::RUNNING, "Running"},
			{IndexRunningStatus::PAUSED, "Paused"},
			{IndexRunningStatus::DISABLED, "Disabled"},
		});
}
