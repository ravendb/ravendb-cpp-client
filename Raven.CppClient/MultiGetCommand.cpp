#include "stdafx.h"
#include "MultiGetCommand.h"
#include "json_utils.h"

namespace ravendb::client::documents::commands::multi_get
{
	std::string MultiGetCommand::get_cached_key(const GetRequest& command,
		std::optional<std::string>& request_url) const
	{
		request_url.emplace(_base_url + command.get_url_and_query());
		std::ostringstream res{};
		res << command.method << "-" << *request_url;
		return res.str();
	}

	void MultiGetCommand::maybe_read_from_cache(GetResponse& get_response, const GetRequest& command) const
	{
		if (get_response.status_code != static_cast<int32_t>(http::HttpStatus::SC_NOT_MODIFIED))
		{
			return;
		}

		std::optional<std::string> dummy{};
		auto cache_key = get_cached_key(command, dummy);
		auto cached_response = std::optional<std::string>{};

		auto cache_item = _cache->get(cache_key, dummy, cached_response);
		get_response.result = std::move(cached_response);
	}

	void MultiGetCommand::maybe_set_cache(const GetResponse& get_response, const GetRequest& command) const
	{
		if (get_response.status_code != static_cast<int32_t>(http::HttpStatus::SC_NOT_MODIFIED))
		{
			return;
		}

		std::optional<std::string> dummy{};
		auto cache_key = get_cached_key(command, dummy);
		auto&& result = get_response.result;
		if (!result)
		{
			return;
		}

		auto change_vector = extensions::HttpExtensions::get_etag_header(get_response.headers);
		if (!change_vector)
		{
			return;
		}

		_cache->set(cache_key, *change_vector, *result);
	}

	MultiGetCommand::MultiGetCommand(std::shared_ptr<http::HttpCache> cache, const std::vector<GetRequest>& commands)
		: _cache(cache)
		, _commands(commands)
	{
		//TODO change later and implement set_raw_response
		//_response_type = http::RavenCommandResponseType::RAW;
	}

	void MultiGetCommand::create_request(impl::CurlHandlesHolder::CurlReference& curl_ref,
		std::shared_ptr<const http::ServerNode> node, std::optional<std::string>& url_ref)
	{
		std::ostringstream url_builder;
		url_builder << node->url << "/databases/" << node->database;

		_base_url = url_builder.str();

		curl_easy_setopt(curl_ref.get(), CURLOPT_POST, 1);
		curl_ref.method = constants::methods::POST;

		nlohmann::json object = nlohmann::json::object();
		nlohmann::json array = nlohmann::json::array();

		using impl::utils::json_utils::set_val_to_json;

		for (const auto& command : _commands.get())
		{
			std::optional<std::string> dummy{};
			auto cached_key = get_cached_key(command, dummy);
			auto cached_change_vector = std::optional<std::string>{};
			auto item = _cache->get(cached_key, cached_change_vector, dummy);
			auto headers = std::unordered_map<std::string, std::string>{};
			if (cached_change_vector)
			{
				std::ostringstream cached_change_vector_header{};
				cached_change_vector_header << '"' << *cached_change_vector << '"';
				headers.insert_or_assign("If-None-Match", cached_change_vector_header.str());
			}
			std::copy(command.headers.cbegin(), command.headers.cend(), std::inserter(headers, headers.end()));

			nlohmann::json temp_object = nlohmann::json::object();

			std::ostringstream temp_url{};
			temp_url << "/databases/" << node->database << command.url;
			set_val_to_json(temp_object, "Url", temp_url.str());
			set_val_to_json(temp_object, "Query", command.query);
			set_val_to_json(temp_object, "Method", command.method);
			set_val_to_json(temp_object, "Headers", headers);

			nlohmann::json content = nullptr;
			if (command.content)
			{
				command.content->write_content(content);
			}
			set_val_to_json(temp_object, "Content", content);

			array.push_back(std::move(temp_object));
		}
		set_val_to_json(object, "Requests", std::move(array));

		auto&& json_str = object.dump();
		curl_easy_setopt(curl_ref.get(), CURLOPT_POSTFIELDSIZE_LARGE, curl_off_t(json_str.size()));
		curl_easy_setopt(curl_ref.get(), CURLOPT_COPYPOSTFIELDS, json_str.c_str());

		curl_ref.headers.emplace(constants::headers::CONTENT_TYPE, "application/json");

		url_builder << "/multi_get";
		url_ref.emplace(url_builder.str());
	}

	bool MultiGetCommand::is_read_request() const
	{
		return false;
	}

	void MultiGetCommand::set_response_raw(const impl::CurlResponse& response)
	{
		//TODO implement
	}

	void MultiGetCommand::set_response(const std::optional<nlohmann::json>& response, bool from_cache)
	{
		auto it = response->find("Results");
		if (it == response->end() || !it->is_array())
		{
			throw_invalid_response();
		}
		_result = std::make_shared<ResultType>();
		std::size_t i = 0;
		for (const auto& result : *it)
		{
			auto get_response = result.get<GetResponse>();
			auto&& command = _commands.get().at(i);
			maybe_set_cache(get_response, command);
			maybe_read_from_cache(get_response, command);

			_result->push_back(std::move(get_response));
			++i;
		}
	}
}
