#pragma once
#include "json_utils.h"
#include "CompanyType.h"
#include "Contact.h"

namespace ravendb::client::tests::infrastructure::entities
{
	struct Company
	{
		double accountsReceivable{};
		std::string id;
		std::string name;
		std::string desc;
		std::string email;
		std::string address1;
		std::string address2;
		std::string address3;
		std::vector<Contact> contacts;
		int32_t phone{};
		CompanyType type = CompanyType::UNSET;
		std::vector<std::string> employeesIds;
	};

	inline void to_json(nlohmann::json& j, const Company& c)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "accountsReceivable", c.accountsReceivable);
		set_val_to_json(j, "name", c.name);
		set_val_to_json(j, "description", c.desc);
		set_val_to_json(j, "address1", c.address1);
		set_val_to_json(j, "address2", c.address2);
		set_val_to_json(j, "address3", c.address3);
		set_val_to_json(j, "contacts", c.contacts);
		set_val_to_json(j, "type", c.type);
		set_val_to_json(j, "employeesIds", c.employeesIds);
		//TODO erase once proper "pluralize" is ready
		j["@metadata"]["@collection"] = "Companies";
	}

	inline void from_json(const nlohmann::json& j, Company& c)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "accountsReceivable", c.accountsReceivable);
		get_val_from_json(j, "name", c.name);
		get_val_from_json(j, "description", c.desc);
		get_val_from_json(j, "address1", c.address1);
		get_val_from_json(j, "address2", c.address2);
		get_val_from_json(j, "address3", c.address3);
		get_val_from_json(j, "contacts", c.contacts);
		get_val_from_json(j, "type", c.type);
		get_val_from_json(j, "employeesIds", c.employeesIds);
	}
}

