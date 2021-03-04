#pragma once
#include "nlohmann/json.hpp"

namespace ravendb::client::http
{
	enum class ReadBalanceBehavior
	{
		NONE,
		ROUND_ROBIN,
		FASTEST_NODE,
		UNSET = -1
	};

	NLOHMANN_JSON_SERIALIZE_ENUM(ReadBalanceBehavior,
		{
			{ReadBalanceBehavior::UNSET, nullptr},
			{ReadBalanceBehavior::NONE, "None"},
			{ReadBalanceBehavior::ROUND_ROBIN, "RoundRobin"},
			{ReadBalanceBehavior::FASTEST_NODE, "FastestNode"}
		});
}
