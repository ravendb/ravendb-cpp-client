#pragma once
#include "RavenCommand.h"
#include "PutResult.h"

namespace ravendb::client::documents::commands
{
	class PutDocumentCommand : public http::RavenCommand<batches::PutResult>
	{
	private:
		const std::string _id;
		const std::optional<std::string> _change_vector;
		std::istringstream _document_stream;

	public:

		~PutDocumentCommand() override = default;

		PutDocumentCommand(std::string id, std::optional<std::string> change_vector, const nlohmann::json& document);
		
		PutDocumentCommand(std::string id, std::optional<std::string> change_vector, const std::string& document);

		void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
			std::optional<std::string>& url_ref) override;

		void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override;

		bool is_read_request() const override;
	};
}
