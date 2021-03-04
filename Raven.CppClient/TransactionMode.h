#pragma once
#include "nlohmann/json.hpp"

namespace ravendb::client::documents::session
{
	enum class TransactionMode
	{
		SINGLE_NODE,
		CLUSTER_WIDE,
		UNSET = -1
	};

	NLOHMANN_JSON_SERIALIZE_ENUM(TransactionMode,
		{
			{TransactionMode::UNSET, nullptr},
			{TransactionMode::SINGLE_NODE, "SingleNode"},
			{TransactionMode::CLUSTER_WIDE, "ClusterWide"}
		})
}
