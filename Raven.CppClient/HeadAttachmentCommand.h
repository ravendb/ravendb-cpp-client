#pragma once
#include "RavenCommand.h"
#include "HttpExtensions.h"

namespace ravendb::client::documents::commands
{
	class HeadAttachmentCommand : public http::RavenCommand<std::string>
	{
	private:
		const std::string _document_id;
		const std::string _name;
		const std::optional<std::string> _change_vector;

	public:
		~HeadAttachmentCommand() override = default;

		HeadAttachmentCommand(std::string document_id, std::string name, std::optional<std::string> change_vector = {});

		void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
			std::optional<std::string>& url_ref) override;

		http::ResponseDisposeHandling process_response(std::shared_ptr<http::HttpCache>,
			const impl::CurlResponse& response,
			const std::string& url) override;

		void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override;

		bool is_read_request() const override;
	};
}
