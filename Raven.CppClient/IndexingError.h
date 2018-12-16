#pragma once

#include "DateTimeOffset.h"
#include "json_utils.h"

namespace ravendb::client::documents::indexes
{
	struct IndexingError
	{
		std::string error{};
		ravendb::client::impl::DateTimeOffset time_stamp{};
		std::string document{};
		std::string action{};

		std::string to_string() const
		{
			std::ostringstream str{};
			str << "Error: " << error << ", Document: " << document << ", Action: " << action;
			return str.str();
		}
	};

	inline void to_json(nlohmann::json& j, const IndexingError& ie)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Error", ie.error);
		set_val_to_json(j, "Timestamp", ie.time_stamp);
		set_val_to_json(j, "Document", ie.document);
		set_val_to_json(j, "Action", ie.action);

	}

	inline void from_json(const nlohmann::json& j, IndexingError& ie)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Error", ie.error);
		get_val_from_json(j, "Timestamp", ie.time_stamp);
		get_val_from_json(j, "Document", ie.document);
		get_val_from_json(j, "Action", ie.action);
	}
}