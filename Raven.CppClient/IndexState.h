#pragma once

namespace ravendb::client::documents::indexes
{
	enum class IndexState
	{
		NORMAL,
		DISABLED,
		IDLE,
		_ERROR,
		UNSET = -1
	};

	NLOHMANN_JSON_SERIALIZE_ENUM(IndexState,
		{
			{IndexState::UNSET, nullptr},
			{IndexState::NORMAL, "Normal"},
			{IndexState::DISABLED, "Disabled"},
			{IndexState::IDLE, "Idle"},
			{IndexState:: _ERROR, "Error"}
		});
}
