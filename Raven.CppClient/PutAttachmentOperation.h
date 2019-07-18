#pragma once
#include "IOperation.h"
#include "AttachmentDetails.h"

namespace ravendb::client::documents::operations::attachments
{
	class PutAttachmentOperation : public IOperation<AttachmentDetails>
	{
	private:
		const std::string _document_id;
		const std::string _name;
		std::istream& _data_stream;
		const std::optional<std::string> _content_type;
		const std::optional<std::string> _change_vector;

	public:
		PutAttachmentOperation(std::string document_id, std::string name,
			std::istream& data_stream,
			std::optional<std::string> content_type = {}, std::optional<std::string> change_vector = {});

		std::unique_ptr<http::RavenCommand<AttachmentDetails>> get_command(std::shared_ptr<IDocumentStore> store,
			std::shared_ptr<conventions::DocumentConventions> conventions, std::shared_ptr<http::HttpCache> cache) const override;

	private:
		class PutAttachmentCommand : public http::RavenCommand<AttachmentDetails>
		{
			const std::string _document_id;
			const std::string _name;
			std::istream& _data_stream;
			const std::optional<std::string> _content_type;
			const std::optional<std::string> _change_vector;

		public:
			~PutAttachmentCommand() override = default;

			PutAttachmentCommand(std::string document_id, std::string name,
				std::istream& data_stream,
				std::optional<std::string> content_type, std::optional<std::string> change_vector);

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override;

			void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override;

			bool is_read_request() const override;
		};
	};
}
