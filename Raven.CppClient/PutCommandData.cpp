#include "stdafx.h"
#include "PutCommandData.h"
#include "json_utils.h"
#include "utils.h"

namespace ravendb::client::documents::commands::batches
{
	nlohmann::json PutCommandData::serialize() const
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;
		nlohmann::json j = nlohmann::json::object();

		set_val_to_json(j, "Id", get_id());
		if (!impl::utils::is_blank(get_change_vector()))
		{
			set_val_to_json(j, "ChangeVector", get_change_vector());
		}
		else
		{
			set_val_to_json(j, "ChangeVector", nullptr);
		}
		set_val_to_json(j, "Document", _document);
		set_val_to_json(j, "Type", get_type());

		return j;
	}
}
