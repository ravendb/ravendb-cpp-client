#include "stdafx.h"
#include "PutAttachmentOperation.h"

namespace ravendb::client::documents::operations::attachments
{
	PutAttachmentOperation::PutAttachmentOperation(std::string document_id, std::string name, std::istream& data_stream,
		std::optional<std::string> content_type, std::optional<std::string> change_vector)
		: _document_id(std::move(document_id))
		, _name(std::move(name))
		, _data_stream(data_stream)
		, _content_type(std::move(content_type))
		, _change_vector(std::move(change_vector))
	{}

	std::unique_ptr<http::RavenCommand<AttachmentDetails>> PutAttachmentOperation::get_command(
		std::shared_ptr<IDocumentStore> store, std::shared_ptr<conventions::DocumentConventions> conventions,
		std::shared_ptr<http::HttpCache> cache) const
	{
		return std::make_unique<PutAttachmentCommand>(_document_id, _name, _data_stream, _content_type, _change_vector);
	}

	PutAttachmentOperation::PutAttachmentCommand::PutAttachmentCommand(std::string document_id, std::string name,
		std::istream& data_stream, std::optional<std::string> content_type, std::optional<std::string> change_vector)
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
		, _data_stream(data_stream)
		, _content_type(std::move(content_type))
		, _change_vector(std::move(change_vector))
	{}

	void PutAttachmentOperation::PutAttachmentCommand::create_request(impl::CurlHandlesHolder::CurlReference& curl_ref,
		std::shared_ptr<const http::ServerNode> node, std::optional<std::string>& url_ref)
	{
		auto curl = curl_ref.get();
		std::ostringstream path_builder;

		path_builder << node->url << "/databases/" << node->database
			<< "/attachments?id=" << http::url_encode(curl_ref, _document_id)
			<< "&name=" << http::url_encode(curl_ref, _name);

		if (_content_type)
		{
			path_builder << "&contentType=" << http::url_encode(curl_ref, *_content_type);
		}

		curl_ref.method = constants::methods::PUT;
		curl_ref.headers.insert_or_assign("Transfer-Encoding", "chunked");

		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, stream_read_callback);
		curl_easy_setopt(curl, CURLOPT_READDATA, &_data_stream);

		add_change_vector_if_not_null(curl_ref, _change_vector);

		url_ref.emplace(path_builder.str());
	}

	void PutAttachmentOperation::PutAttachmentCommand::set_response(const std::optional<nlohmann::json>& response,
		bool from_cache)
	{
		_result = std::make_shared<ResultType>(response->get<ResultType>());
	}

	bool PutAttachmentOperation::PutAttachmentCommand::is_read_request() const
	{
		return false;
	}
}
