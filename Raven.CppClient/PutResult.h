#pragma once
#include "stdafx.h"

namespace  ravendb::client::documents::commands::batches
{
	struct PutResult
	{
		std::string id{};
		std::string change_vector{};
	};

	inline void from_json(const nlohmann::json& j, PutResult& p)
	{
		p.id = j.at("Id").get<std::string>();
		p.change_vector = j.at("ChangeVector").get<std::string>();
	}
}