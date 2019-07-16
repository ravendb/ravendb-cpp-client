#include "stdafx.h"
#include "DeleteDocumentCommand.h"

namespace ravendb::client::documents::commands
{
	DeleteDocumentCommand::DeleteDocumentCommand(std::string id, std::optional<std::string> change_vector)
		: _id([&id]
	{
		if (id.empty()) throw std::invalid_argument("id cannot be empty");
		return std::move(id);
	}())
		, _change_vector(std::move(change_vector))
	{}

	void DeleteDocumentCommand::create_request(impl::CurlHandlesHolder::CurlReference& curl_ref,
		std::shared_ptr<const http::ServerNode> node, std::optional<std::string>& url_ref)
	{
		std::ostringstream path_builder;
		path_builder << node->url << "/databases/" << node->database
			<< "/docs?id=" << http::url_encode(curl_ref, _id);

		curl_easy_setopt(curl_ref.get(), CURLOPT_CUSTOMREQUEST, "DELETE");
		curl_ref.method = constants::methods::DELETE_;

		add_change_vector_if_not_null(curl_ref, _change_vector);

		url_ref.emplace(path_builder.str());
	}
}
