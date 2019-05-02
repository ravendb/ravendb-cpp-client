#pragma once
#include "RavenCommand.h"
#include "PutResult.h"

namespace ravendb::client::documents::commands
{
	class PutDocumentCommand : public http::RavenCommand<batches::PutResult>
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

		void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
			std::optional<std::string>& url_ref) override
		{
			auto curl = curl_ref.get();
			std::ostringstream path_builder;

			path_builder << node->url << "/databases/" << node->database
				<< "/docs?id=" << url_encode(curl_ref, _id);

			curl_easy_setopt(curl, CURLOPT_PUT, 1L);
			curl_ref.method = constants::methods::PUT;

			curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
			curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
			curl_easy_setopt(curl, CURLOPT_READDATA, &_document);
			curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)_document.length());

			curl_ref.headers.insert_or_assign(constants::headers::CONTENT_TYPE, "application/json");
			add_change_vector_if_not_null(curl_ref, _change_vector);

			url_ref.emplace(path_builder.str());
		}

		void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override
		{
			_result = std::make_shared<ResultType>(response->get<ResultType>());
		}

		bool is_read_request() const noexcept override
		{
			return false;
		}
	};
}
