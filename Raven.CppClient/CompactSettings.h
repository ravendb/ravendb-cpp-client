#pragma once
#include "json.hpp"
#include "json_utils.h"

namespace  ravendb::client::serverwide
{
	struct CompactSettings
	{
		std::string database_name{};
		std::shared_ptr<const http::ServerNode> node{};
		bool documents{};
		std::vector<std::string> indexes{};
	};

	inline void to_json(nlohmann::json& j, const CompactSettings& cs)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		if (cs.node)
		{
			set_val_to_json(j, "ServerNode", *cs.node);
		}
		else
		{
			set_val_to_json(j, "ServerNode", nullptr);
		}
		set_val_to_json(j, "DatabaseName", cs.database_name);
		set_val_to_json(j, "Documents", cs.documents);
		set_val_to_json(j, "Indexes", cs.indexes);
	}
}
