#pragma once
#include "json.hpp"

namespace ravendb::client::documents::indexes
{
	enum class IndexLockMode
	{
		UNLOCK,
		LOCKED_IGNORE,
		LOCKED_ERROR,
		UNSET = -1
	};

	NLOHMANN_JSON_SERIALIZE_ENUM(IndexLockMode,
	{
		{IndexLockMode::UNSET, nullptr},
		{IndexLockMode::UNLOCK, "Unlock"},
		{IndexLockMode::LOCKED_IGNORE, "LockedIgnore"},
		{IndexLockMode::LOCKED_ERROR, "LockedError"},
	});
}
