#pragma once

namespace ravendb::client::documents::session
{
	enum class ConcurrencyCheckMode
	{
		AUTO,
		FORCED,
		DISABLED,
		UNSET = -1
	};

	NLOHMANN_JSON_SERIALIZE_ENUM(ConcurrencyCheckMode,
		{
			{ConcurrencyCheckMode::UNSET, nullptr},
			{ConcurrencyCheckMode::AUTO, "Auto"},
			{ConcurrencyCheckMode::FORCED, "Forced"},
			{ConcurrencyCheckMode::DISABLED, "Disabled"}
		});
}
