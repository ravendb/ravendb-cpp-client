#include "stdafx.h"
#include "HeadDocumentCommand.h"

namespace ravendb::client::documents::commands
{
	HeadDocumentCommand::HeadDocumentCommand(std::string id, std::optional<std::string> change_vector)
		: _id(std::move(id))
		, _change_vector(std::move(change_vector))
	{}

	void HeadDocumentCommand::create_request(impl::CurlHandlesHolder::CurlReference& curl_ref,
		std::shared_ptr<const http::ServerNode> node, std::optional<std::string>& url_ref)
	{
		auto curl = curl_ref.get();
		std::ostringstream path_builder;

		path_builder << node->url << "/databases/" << node->database
			<< "/docs?id=" << http::url_encode(curl_ref, _id);

		curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "HEAD");;
		curl_ref.method = constants::methods::HEAD;

		if (_change_vector)
		{
			curl_ref.headers.insert_or_assign("If-None-Match", *_change_vector);
		}

		url_ref.emplace(path_builder.str());
	}

	http::ResponseDisposeHandling HeadDocumentCommand::process_response(std::shared_ptr<http::HttpCache>,
		const impl::CurlResponse& response, const std::string& url)
	{
		const auto status_code = static_cast<http::HttpStatus>(response.status_code);

		if (status_code == http::HttpStatus::SC_NOT_MODIFIED)
		{
			if (_change_vector)
			{
				_result = std::make_shared<std::string>(*_change_vector);
			}
			else
			{
				_result.reset();
			}
			return http::ResponseDisposeHandling::AUTOMATIC;
		}

		if (status_code == http::HttpStatus::SC_NOT_FOUND)
		{
			_result.reset();
			return  http::ResponseDisposeHandling::AUTOMATIC;
		}

		_result = std::make_shared<std::string>(extensions::HttpExtensions::get_required_etag_header(response));
		return  http::ResponseDisposeHandling::AUTOMATIC;
	}

	void HeadDocumentCommand::set_response(const std::optional<nlohmann::json>& response, bool from_cache)
	{
		if (response)
		{
			throw_invalid_response();
		}
		_result.reset();
	}

	bool HeadDocumentCommand::is_read_request() const
	{
		return false;
	}
}
