#include "stdafx.h"
#include "GetResponse.h"
#include "HttpStatus.h"
#include "json_utils.h"

namespace ravendb::client::documents::commands::multi_get
{
	bool GetResponse::request_has_errors() const
	{
		bool are_errors = false;

		switch (static_cast<http::HttpStatus>(status_code))
		{
		case http::HttpStatus::SC_OK:
		case http::HttpStatus::SC_CREATED:
		case http::HttpStatus::SC_NON_AUTHORITATIVE_INFORMATION:
		case http::HttpStatus::SC_NO_CONTENT:
		case http::HttpStatus::SC_NOT_MODIFIED:
		case http::HttpStatus::SC_NOT_FOUND:
			break;
		default:
			are_errors = status_code != 0;
			break;
		}
		return are_errors;
	}

	void from_json(const nlohmann::json& j, GetResponse& gr)
	{
		using impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Headers", gr.headers);
		if (auto it = j.find("Result");
			it != j.end())
		{
			gr.result.emplace(it->dump());
		}
		get_val_from_json(j, "StatusCode", gr.status_code);
		get_val_from_json(j, "ForceRetry", gr.force_retry);
	}
}
