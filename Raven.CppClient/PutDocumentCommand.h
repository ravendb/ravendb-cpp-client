#pragma once
#include "stdafx.h"
#include "RavenCommand.h"
#include "PutResult.h"

using 
	ravendb::client::http::RavenCommand,
	ravendb::client::documents::commands::batches::PutResult,
	ravendb::client::http::ServerNode;

namespace ravendb::client::documents::commands
{
	class PutDocumentCommand : public RavenCommand<PutResult>
	{
	private:
		std::string _id;
		std::optional<std::string> _change_vector;
		std::string _document;

	public:

		~PutDocumentCommand() override = default;

		PutDocumentCommand(std::string id, std::optional<std::string> change_vector, nlohmann::json document)
			: PutDocumentCommand(std::move(id), std::move(change_vector), document.dump())
		{}
		
		PutDocumentCommand(std::string id, std::optional<std::string> change_vector, std::string document)
			: _id(std::move(id))
			, _change_vector(std::move(change_vector))
			, _document(std::move(document))
		{}

		void create_request(CURL* curl, const ServerNode& node, std::string& url) override
		{
			std::ostringstream pathBuilder;
			pathBuilder << node.url << "/databases/" << node.database
				<< "/docs?id=" << ravendb::client::impl::utils::url_escape(curl, _id);

			curl_easy_setopt(curl, CURLOPT_READFUNCTION, ravendb::client::impl::utils::read_callback);
			curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
			curl_easy_setopt(curl, CURLOPT_PUT, 1L);
			curl_easy_setopt(curl, CURLOPT_READDATA, &_document);
			curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)_document.length());

			add_change_vector_if_not_null(curl, _change_vector);

			url = pathBuilder.str();
		}

		void set_response(CURL* curl, const nlohmann::json& response, bool from_cache) override
		{
				_result = response;
		}

		bool is_read_request() const noexcept override
		{
			return false;
		}
	};
}
