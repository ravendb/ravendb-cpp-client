#pragma once
#include "json_utils.h"
#include "CompanyType.h"
#include "Contact.h"

namespace ravendb::client::tests::infrastructure::entities
{
	struct Company
	{
		double accounts_receivable{};
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
		std::vector<std::string> employees_ids;
	};

	inline void to_json(nlohmann::json& j, const Company& c)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Id", c.id);
		set_val_to_json(j, "Name", c.name);
		set_val_to_json(j, "Description", c.desc);
		set_val_to_json(j, "Address1", c.address1);
		set_val_to_json(j, "Address2", c.address2);
		set_val_to_json(j, "Address3", c.address3);
		set_val_to_json(j, "Contacts", c.contacts);
		set_val_to_json(j, "Type", c.type);
		set_val_to_json(j, "EmployeesIds", c.employees_ids);
		j["@metadata"]["@collection"] = "Companies";
	}

	inline void from_json(const nlohmann::json& j, Company& c)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Id", c.id);
		get_val_from_json(j, "Name", c.name);
		get_val_from_json(j, "Description", c.desc);
		get_val_from_json(j, "Address1", c.address1);
		get_val_from_json(j, "Address2", c.address2);
		get_val_from_json(j, "Address3", c.address3);
		get_val_from_json(j, "Contacts", c.contacts);
		get_val_from_json(j, "Type", c.type);
		get_val_from_json(j, "EmployeesIds", c.employees_ids);
	}
}

