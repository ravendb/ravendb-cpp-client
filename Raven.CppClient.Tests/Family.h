#pragma once
#include "json_utils.h"

namespace ravendb::client::tests::infrastructure::entities
{
	struct Family
	{
		std::vector<std::string> names;

		friend bool operator==(const Family& lhs, const Family& rhs)
		{
			return lhs.names == rhs.names;
		}
	};

	inline void to_json(nlohmann::json& j, const Family& f)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Names", f.names);
	}

	inline void from_json(const nlohmann::json& j, Family& f)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Name", f.names);
	}

	struct Member
	{
		std::string name{};
		int32_t age{};
	};

	inline void to_json(nlohmann::json& j, const Member& m)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Name", m.name);
		set_val_to_json(j, "Age", m.age);
	}

	inline void from_json(const nlohmann::json& j, Member& m)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Name", m.name);
		get_val_from_json(j, "Age", m.age);
	}

	inline void to_json(nlohmann::json& j, const std::shared_ptr<Member>& m)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		if(!m)
		{
			throw std::invalid_argument("m is null");
		}
		set_val_to_json(j, "Name", m->name);
		set_val_to_json(j, "Age", m->age);
	}

	inline void from_json(const nlohmann::json& j, std::shared_ptr<Member>& m)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		m = std::make_shared<Member>();
		get_val_from_json(j, "Name", m->name);
		get_val_from_json(j, "Age", m->age);
	}

	struct FamilyMembers
	{
		std::vector<std::shared_ptr<Member>> members{};
	};

	inline void to_json(nlohmann::json& j, const FamilyMembers& fm)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Members", fm.members);
	}

	inline void from_json(const nlohmann::json& j, FamilyMembers& fm)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Members", fm.members);
	}
}

