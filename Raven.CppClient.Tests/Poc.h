#pragma once
#include "json_utils.h"
#include "User.h"

namespace ravendb::client::tests::infrastructure::entities
{
	struct Poc
	{
		std::optional<std::string> name;
		std::optional<User> obj;
	};

	inline void to_json(nlohmann::json& j, const Poc& p)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Name", p.name);
		set_val_to_json(j, "Obj", p.obj);
	}

	inline void from_json(const nlohmann::json& j, Poc& p)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Name", p.name);
		get_val_from_json(j, "Obj", p.obj);
	}
}

