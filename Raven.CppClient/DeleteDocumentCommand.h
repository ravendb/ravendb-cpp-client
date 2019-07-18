#pragma once
#include "VoidRavenCommand.h"

namespace ravendb::client::documents::commands
{
	class DeleteDocumentCommand : public http::VoidRavenCommand
	{
	private:
		std::string _id;
		std::optional<std::string> _change_vector;
		
	public:
		~DeleteDocumentCommand() override = default;
		
		explicit DeleteDocumentCommand(std::string id, std::optional<std::string> change_vector = {});

		void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
			std::optional<std::string>& url_ref) override;
	};
}
