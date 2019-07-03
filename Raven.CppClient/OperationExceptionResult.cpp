#include "stdafx.h"
#include "OperationExceptionResult.h"

namespace ravendb::client::documents::operations
{
	void from_json(const nlohmann::json& j, OperationExceptionResult& oer)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Type", oer.type);
		get_val_from_json(j, "Message", oer.message);
		get_val_from_json(j, "Error", oer.error);
		get_val_from_json(j, "StatusCode", oer.status_code);
	}
}
