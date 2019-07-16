#include "stdafx.h"
#include "PutDocumentCommand.h"

namespace ravendb::client::documents::commands
{
	PutDocumentCommand::PutDocumentCommand(std::string id, std::optional<std::string> change_vector,
		const nlohmann::json& document)
		: _id(std::move(id))
		, _change_vector(std::move(change_vector))
		, _document_stream(document.dump())
	{}

	PutDocumentCommand::PutDocumentCommand(std::string id, std::optional<std::string> change_vector,
		const std::string& document)
		: _id(std::move(id))
		, _change_vector(std::move(change_vector))
		, _document_stream(document)
	{}

	void PutDocumentCommand::create_request(impl::CurlHandlesHolder::CurlReference& curl_ref,
		std::shared_ptr<const http::ServerNode> node, std::optional<std::string>& url_ref)
	{
		auto curl = curl_ref.get();
		std::ostringstream path_builder;

		path_builder << node->url << "/databases/" << node->database
			<< "/docs?id=" << http::url_encode(curl_ref, _id);

		curl_ref.method = constants::methods::PUT;
		curl_ref.headers.insert_or_assign("Transfer-Encoding", "chunked");
		curl_ref.headers.emplace(constants::headers::CONTENT_TYPE, "application/json");

		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, stream_read_callback);
		curl_easy_setopt(curl, CURLOPT_READDATA, &_document_stream);

		add_change_vector_if_not_null(curl_ref, _change_vector);

		_document_stream.clear();
		_document_stream.seekg(0);

		url_ref.emplace(path_builder.str());
	}

	void PutDocumentCommand::set_response(const std::optional<nlohmann::json>& response, bool from_cache)
	{
		_result = std::make_shared<ResultType>(response->get<ResultType>());
	}

	bool PutDocumentCommand::is_read_request() const
	{
		return false;
	}
}
