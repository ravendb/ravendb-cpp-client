#include "stdafx.h"
#include "IndexingError.h"
#include "json_utils.h"

namespace ravendb::client::documents::indexes
{
	std::string IndexingError::to_string() const
	{
		std::ostringstream str{};
		str << "Error: " << error << ", Document: " << document << ", Action: " << action;
		return str.str();
	}

	void to_json(nlohmann::json& j, const IndexingError& ie)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Error", ie.error);
		set_val_to_json(j, "Timestamp", ie.time_stamp);
		set_val_to_json(j, "Document", ie.document);
		set_val_to_json(j, "Action", ie.action);
	}

	void from_json(const nlohmann::json& j, IndexingError& ie)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Error", ie.error);
		get_val_from_json(j, "Timestamp", ie.time_stamp);
		get_val_from_json(j, "Document", ie.document);
		get_val_from_json(j, "Action", ie.action);
	}
}
