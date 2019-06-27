#pragma once
#include "stdafx.h"
#include "RavenCommand.h"
#include "json_utils.h"

namespace ravendb::client::documents::commands
{

	class GetNextOperationIdCommand :public http::RavenCommand<int64_t>
	{
	public:

		~GetNextOperationIdCommand() override = default;
		GetNextOperationIdCommand() = default;

		void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
			std::optional<std::string>& url_ref) override
		{
			std::ostringstream url_builder;
			url_builder << node->url << "/databases/" << node->database
				<< "/operations/next-operation-id";

			curl_easy_setopt(curl_ref.get(), CURLOPT_HTTPGET, 1);
			curl_ref.method = constants::methods::GET;

			url_ref.emplace(url_builder.str());
		}

		void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override
		{
			_result = std::make_shared<ResultType>();
			if(! impl::utils::json_utils::get_val_from_json(*response, "Id", *_result))
			{
				_result.reset();
			}
		}

		bool is_read_request() const override
		{
			return false;// disable caching
		}
	};
}
