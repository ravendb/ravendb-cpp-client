#include "stdafx.h"
#include "BatchCommandResult.h"
#include "json_utils.h"

namespace  ravendb::client::json
{
	void from_json(const nlohmann::json & j, BatchCommandResult & bcr)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Results", bcr.results);
		get_val_from_json(j, "TransactionIndex", bcr.transaction_index);
	}

	void to_json(nlohmann::json & j, const BatchCommandResult & bcr)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Results", bcr.results);
		set_val_to_json(j, "TransactionIndex", bcr.transaction_index);
	}
}
