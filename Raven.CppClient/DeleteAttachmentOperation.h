#pragma once
#include "IVoidOperation.h"
#include "VoidRavenCommand.h"

namespace ravendb::client::documents::operations::attachments
{
	class DeleteAttachmentOperation : public IVoidOperation
	{
	private:
		const std::string _document_id;
		const std::string _name;
		const std::optional<std::string> _change_vector;

	public:
		DeleteAttachmentOperation(std::string document_id, std::string name, std::optional<std::string> change_vector = {});

		std::unique_ptr<http::RavenCommand<void>> get_command(std::shared_ptr<IDocumentStore> store,
			std::shared_ptr<conventions::DocumentConventions> conventions,
		    std::shared_ptr<http::HttpCache> cache) const override;

	private:
		class DeleteAttachmentCommand : public http::VoidRavenCommand
		{
			const std::string _document_id;
			const std::string _name;
			const std::optional<std::string> _change_vector;

		public:
			~DeleteAttachmentCommand() override = default;

			DeleteAttachmentCommand(std::string document_id, std::string name, std::optional<std::string> change_vector);

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref,
				std::shared_ptr<const http::ServerNode> node,
			    std::optional<std::string>& url_ref) override;
		};
	};
}
