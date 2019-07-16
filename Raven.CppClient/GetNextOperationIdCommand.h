#pragma once
#include "RavenCommand.h"

namespace ravendb::client::documents::commands
{
	class GetNextOperationIdCommand :public http::RavenCommand<int64_t>
	{
	public:

		~GetNextOperationIdCommand() override = default;
		GetNextOperationIdCommand() = default;

		void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
			std::optional<std::string>& url_ref) override;

		void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override;

		bool is_read_request() const override;
	};
}
