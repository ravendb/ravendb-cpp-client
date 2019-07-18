#include "stdafx.h"
#include "HeadAttachmentCommand.h"
#include "utils.h"

namespace ravendb::client::documents::commands
{
	HeadAttachmentCommand::HeadAttachmentCommand(std::string document_id, std::string name,
		std::optional<std::string> change_vector)
		: _document_id([&]
	{
		if (impl::utils::is_blank(document_id))
		{
			throw std::invalid_argument("document_id cannot be blank");
		}
		return std::move(document_id);
	}())
		, _name([&]
	{
		if (impl::utils::is_blank(name))
		{
			throw std::invalid_argument("name cannot be blank");
		}
		return std::move(name);
	}())
		, _change_vector(std::move(change_vector))
	{}

	void HeadAttachmentCommand::create_request(impl::CurlHandlesHolder::CurlReference& curl_ref,
		std::shared_ptr<const http::ServerNode> node, std::optional<std::string>& url_ref)
	{
		auto curl = curl_ref.get();
		std::ostringstream path_builder;

		path_builder << node->url << "/databases/" << node->database
			<< "/attachments?id=" << http::url_encode(curl_ref, _document_id)
			<< "&name=" << http::url_encode(curl_ref, _name);

		curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "HEAD");;
		curl_ref.method = ravendb::client::constants::methods::HEAD;

		if (_change_vector)
		{
			curl_ref.headers.insert_or_assign("If-None-Match", *_change_vector);
		}

		url_ref.emplace(path_builder.str());
	}

	http::ResponseDisposeHandling HeadAttachmentCommand::process_response(std::shared_ptr<http::HttpCache>,
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

	void HeadAttachmentCommand::set_response(const std::optional<nlohmann::json>& response, bool from_cache)
	{
		if (response)
		{
			throw_invalid_response();
		}
		_result.reset();
	}

	bool HeadAttachmentCommand::is_read_request() const
	{
		return false;
	}
}
