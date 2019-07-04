#pragma once
#include "IOperation.h"
#include "AttachmentDetails.h"
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
			documents::attachments::AttachmentType type, std::optional<std::string> change_vector = {})
			: _document_id(std::move(document_id))
			, _name(std::move(name))
			, _type(type)
			, _change_vector(std::move(change_vector))
		{}

		std::unique_ptr<http::RavenCommand<AttachmentResult>> get_command(std::shared_ptr<IDocumentStore> store,
			std::shared_ptr<conventions::DocumentConventions> conventions, std::shared_ptr<http::HttpCache> cache) const override
		{
			return std::make_unique<GetAttachmentCommand>(_document_id, _name, _type, _change_vector);
		}

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
				documents::attachments::AttachmentType type, std::optional<std::string> change_vector)
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
				, _type(type)
				, _change_vector([&]
			{
				if(_type != documents::attachments::AttachmentType::DOCUMENT && !change_vector)
				{
					throw std::invalid_argument("Change vector cannot be null for attachment type " + nlohmann::json(type).dump());
				}
				return std::move(change_vector);
			}())
			{
				_response_type = http::RavenCommandResponseType::EMPTY;
			}

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override
			{
				auto curl = curl_ref.get();
				std::ostringstream path_builder;

				path_builder << node->url << "/databases/" << node->database
					<< "/attachments?id=" << http::url_encode(curl_ref, _document_id)
					<< "&name=" << http::url_encode(curl_ref, _name);

				if (_type == documents::attachments::AttachmentType::REVISION)
				{

					curl_ref.method = constants::methods::POST;
					curl_ref.headers.insert_or_assign("Transfer-Encoding", "chunked");
					curl_ref.headers.emplace(constants::headers::CONTENT_TYPE, "application/json");

					curl_easy_setopt(curl_ref.get(), CURLOPT_POST, 1);

					nlohmann::json entity = nlohmann::json::object_t();
					impl::utils::json_utils::set_val_to_json(entity, "Type", "Revision");
					impl::utils::json_utils::set_val_to_json(entity, "ChangeVector", _change_vector);

					auto&& json_str = entity.dump();

					curl_easy_setopt(curl_ref.get(), CURLOPT_POSTFIELDSIZE_LARGE, curl_off_t(json_str.size()));
					curl_easy_setopt(curl_ref.get(), CURLOPT_COPYPOSTFIELDS, json_str.c_str());
				}
				else
				{
					curl_easy_setopt(curl_ref.get(), CURLOPT_HTTPGET, 1);
					curl_ref.method = constants::methods::GET;
				}

				url_ref.emplace(path_builder.str());
			}

			http::ResponseDisposeHandling process_response(std::shared_ptr<http::HttpCache> cache,
				const impl::CurlResponse& response, const std::string& url) override
			{
				auto&& content_type = response.headers.at(constants::headers::CONTENT_TYPE);
				auto&& change_vector = extensions::HttpExtensions::get_etag_header(response);
				auto&& hash = response.headers.at("Attachment-Hash");
				int64_t size{ 0 };

				if (auto it = response.headers.find("Attachment-Size");
					it != response.headers.end())
				{
					std::istringstream(it->second) >> size;
				}

				auto&& attachment_details = AttachmentDetails();
				attachment_details.content_type = content_type;
				attachment_details.name = _name;
				attachment_details.hash = hash;
				attachment_details.size = size;
				attachment_details.change_vector = std::move(change_vector);
				attachment_details.document_id = _document_id;

				_result = std::make_shared<AttachmentResult>(response, std::move(attachment_details));

				return http::ResponseDisposeHandling::MANUALLY;
			}
			bool is_read_request() const override
			{
				return true;
			}
		};
	};
}
