#pragma once
#include "IOperation.h"
#include "AttachmentType.h"
#include "AttachmentResult.h"

namespace ravendb::client::documents::operations::attachments
{
	class GetAttachmentOperation : public IOperation<AttachmentResult>
	{
	private:
		const std::string _document_id;
		const std::string _name;
		const documents::attachments::AttachmentType _type;
		const std::optional<std::string> _change_vector;

	public:
		GetAttachmentOperation(std::string document_id, std::string name,
			documents::attachments::AttachmentType type, std::optional<std::string> change_vector = {});

		std::unique_ptr<http::RavenCommand<AttachmentResult>> get_command(std::shared_ptr<IDocumentStore> store,
			std::shared_ptr<conventions::DocumentConventions> conventions, std::shared_ptr<http::HttpCache> cache) const override;

	private:
		class GetAttachmentCommand : public http::RavenCommand<AttachmentResult>
		{
			const std::string _document_id;
			const std::string _name;
			const documents::attachments::AttachmentType _type;
			const std::optional<std::string> _change_vector;

		public:
			~GetAttachmentCommand() override = default;

			GetAttachmentCommand(std::string document_id, std::string name,
				documents::attachments::AttachmentType type, std::optional<std::string> change_vector);

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override;

			http::ResponseDisposeHandling process_response(std::shared_ptr<http::HttpCache> cache,
				const impl::CurlResponse& response, const std::string& url) override;

			bool is_read_request() const override;
		};
	};
}
