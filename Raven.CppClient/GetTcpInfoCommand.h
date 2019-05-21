#pragma once
#include "RavenCommand.h"
#include "TcpConnectionInfo.h"

namespace ravendb::client::http
{
	struct ServerNode;
}

namespace ravendb::client::serverwide::commands
{
	class GetTcpInfoCommand :public http::RavenCommand<TcpConnectionInfo>
	{
	private:
		const std::string _tag;
		const std::optional<std::string> _db_name;
		std::shared_ptr<const http::ServerNode> _requested_node{};

	public:
		~GetTcpInfoCommand() override;

		explicit GetTcpInfoCommand(std::string tag, std::optional<std::string> db_name = {});

		void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
			std::optional<std::string>& url_ref) override;

		std::shared_ptr<const http::ServerNode> get_requested_node() const;
		void set_requested_node(std::shared_ptr<const http::ServerNode> requested_node);

		void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override;

		bool is_read_request() const override;
	};
}
