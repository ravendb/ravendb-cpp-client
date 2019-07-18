#include "stdafx.h"
#include "ClientConfiguration.h"
#include "json_utils.h"

namespace ravendb::client::documents::operations::configuration
{
	void from_json(const nlohmann::json& j, ClientConfiguration& cc)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Etag", cc.etag);
		get_val_from_json(j, "Disabled", cc.disabled);
		get_val_from_json(j, "MaxNumberOfRequestsPerSession", cc.max_number_of_requests_per_session);
		get_val_from_json(j, "ReadBalanceBehavior", cc.read_balance_behaviour);
	}

	void to_json(nlohmann::json& j, const ClientConfiguration& cc)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Etag", cc.etag);
		set_val_to_json(j, "Disabled", cc.disabled);
		set_val_to_json(j, "MaxNumberOfRequestsPerSession", cc.max_number_of_requests_per_session);
		set_val_to_json(j, "ReadBalanceBehavior", cc.read_balance_behaviour);
	}
}
