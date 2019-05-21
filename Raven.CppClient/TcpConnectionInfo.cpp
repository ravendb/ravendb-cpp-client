#include "stdafx.h"
#include "TcpConnectionInfo.h"
#include "json_utils.h"

namespace ravendb::client::serverwide::commands
{
	void to_json(nlohmann::json& j, const TcpConnectionInfo& tci)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Port", tci.port);
		set_val_to_json(j, "Url", tci.url);
		set_val_to_json(j, "Certificate", tci.certificate);
	}

	void from_json(const nlohmann::json& j, TcpConnectionInfo& tci)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Port", tci.port);
		get_val_from_json(j, "Url", tci.url);
		get_val_from_json(j, "Certificate", tci.certificate);
	}
}
