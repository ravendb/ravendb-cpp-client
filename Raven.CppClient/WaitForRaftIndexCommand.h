#pragma once
#include "RavenCommand.h"

namespace ravendb::client::serverwide::commands
{
	class WaitForRaftIndexCommand :public http::RavenCommand<void>
	{
	private:
		int64_t _rachis_index;
		std::shared_ptr<const http::ServerNode> _requested_node{};

	public:
		~WaitForRaftIndexCommand() override;
		explicit WaitForRaftIndexCommand(int64_t rachis_index);

		void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, 
			std::shared_ptr<const http::ServerNode> node,
			std::optional<std::string>& url_ref) override;

		std::shared_ptr<const http::ServerNode> get_requested_node() const;
		void set_requested_node(std::shared_ptr<const http::ServerNode> requested_node);

		void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override;

		bool is_read_request() const override;
	};
}