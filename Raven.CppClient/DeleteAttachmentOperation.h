#pragma once
#include "IVoidOperation.h"
#include "VoidRavenCommand.h"
#include "DeleteByQueryOperation.h"

namespace ravendb::client::documents::operations::attachments
{
	class DeleteAttachmentOperation : public IVoidOperation
	{
	private:
		const std::string _document_id;
		const std::string _name;
		const std::optional<std::string> _change_vector;

	public:
		DeleteAttachmentOperation(std::string document_id, std::string name, std::optional<std::string> change_vector = {})
			: _document_id(std::move(document_id))
			, _name(std::move(name))
			, _change_vector(std::move(change_vector))
		{}

		std::unique_ptr<http::RavenCommand<void>> get_command(std::shared_ptr<IDocumentStore> store,
			std::shared_ptr<conventions::DocumentConventions> conventions,
			std::shared_ptr<http::HttpCache> cache) const override
		{
			return std::make_unique<DeleteAttachmentCommand>(_document_id, _name, _change_vector);
		}

	private:
		class DeleteAttachmentCommand : public http::VoidRavenCommand
		{
			const std::string _document_id;
			const std::string _name;
			const std::optional<std::string> _change_vector;

		public:
			~DeleteAttachmentCommand() override = default;

			DeleteAttachmentCommand(std::string document_id, std::string name, std::optional<std::string> change_vector)
				: _document_id([&]()
			{
				if (impl::utils::is_blank(document_id))
				{
					throw std::invalid_argument("'document_id' cannot be empty");
				}
				return std::move(document_id);
			}())
				, _name([&]()
			{
				if (impl::utils::is_blank(name))
				{
					throw std::invalid_argument("'name' cannot be empty");
				}
				return std::move(name);
			}())
				, _change_vector(std::move(change_vector))
			{}

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override
			{
				std::ostringstream path_builder;

				path_builder << node->url << "/databases/" << node->database
					<< "/attachments?id=" << http::url_encode(curl_ref, _document_id)
					<< "&name=" << http::url_encode(curl_ref, _name);

				curl_easy_setopt(curl_ref.get(), CURLOPT_CUSTOMREQUEST, "DELETE");
				curl_ref.method = constants::methods::DELETE_;

				add_change_vector_if_not_null(curl_ref, _change_vector);

				url_ref.emplace(path_builder.str());
			}
		};
	};
}
