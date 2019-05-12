#include "stdafx.h"
#include "NextHiLoCommand.h"

namespace ravendb::client::documents::commands
{
	NextHiLoCommand::NextHiLoCommand(std::string tag,
		int64_t last_batch_size,
		impl::DateTimeOffset last_range_at,
		std::string identity_part_separator,
		int64_t last_range_max)
		: _tag(std::move(tag))
		, _last_batch_size(last_batch_size)
		, _last_range_at(last_range_at)
		, _identity_parts_separator(std::move(identity_part_separator))
		, _last_range_max(last_range_max)
	{}

	void NextHiLoCommand::create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
		std::optional<std::string>& url_ref)
	{
		std::ostringstream path_builder{};

		path_builder << node->url << "/databases/" << node->database <<
			"/hilo/next?tag=" << _tag <<
			"&lastBatchSize=" << _last_batch_size <<
			"&lastRangeAt=" << _last_range_at.to_string() <<
			"&identityPartsSeparator=" << _identity_parts_separator <<
			"&lastMax=" << _last_range_max;

		curl_easy_setopt(curl_ref.get(), CURLOPT_HTTPGET, 1);
		curl_ref.method = constants::methods::GET;

		url_ref.emplace(path_builder.str());
	}

	void NextHiLoCommand::set_response(const std::optional<nlohmann::json>& response, bool from_cache)
	{
		_result = std::make_shared<ResultType>(response->get<ResultType>());
	}

	bool NextHiLoCommand::is_read_request() const
	{
		return true;
	}
}
