#pragma once
#include "RavenCommand.h"
#include "GetResponse.h"
#include "HttpCache.h"
#include "GetRequest.h"

namespace ravendb::client::documents::commands::multi_get
{
	class MultiGetCommand : public http::RavenCommand<std::vector<GetResponse>>
	{
	private:
		const std::reference_wrapper <http::HttpCache> _cache;
		const std::reference_wrapper<const std::vector<GetRequest>> _commands;

		std::string _base_url{};

	public:
		MultiGetCommand(http::HttpCache& cache, const std::vector<GetRequest>& commands)
			: _cache(cache)
			, _commands(commands)
		{
			//TODO change later and implement set_raw_response
			//_response_type = http::RavenCommandResponseType::RAW;
		}

		void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
			std::optional<std::string>& url_ref) override
		{
			std::ostringstream url_builder;
			url_builder << node->url << "/databases/" << node->database;

			_base_url = url_builder.str();

			curl_easy_setopt(curl_ref.get(), CURLOPT_HTTPPOST, 1);
			curl_ref.method = constants::methods::POST;

			//TODO use the cache

			nlohmann::json object = nlohmann::json::object();
			nlohmann::json array = nlohmann::json::array();

			using impl::utils::json_utils::set_val_to_json;

			for(const auto& command : _commands.get())
			{
				nlohmann::json temp_object = nlohmann::json::object();

				std::ostringstream temp_url{};
				temp_url << "/databases/" << node->database + command.url;
				set_val_to_json(temp_object, "Url", temp_url.str());
				set_val_to_json(temp_object, "Query", command.query);
				set_val_to_json(temp_object, "Method", command.method);
				set_val_to_json(temp_object, "Headers", command.headers);

				nlohmann::json content = nullptr;
				if(command.content)
				{
					command.content->write_content(content);
				}
				set_val_to_json(temp_object, "Content", content);

				array.push_back(std::move(temp_object));
			}
			set_val_to_json(object, "Requests", std::move(array));
			curl_easy_setopt(curl_ref.get(), CURLOPT_COPYPOSTFIELDS, object.dump().c_str());

			curl_ref.headers.emplace(constants::headers::CONTENT_TYPE, "application/json");

			url_builder << "/multi_get";
			url_ref.emplace(url_builder.str());
		}

		bool is_read_request() const override
		{
			return false;
		}

		void set_response_raw(const impl::CurlResponse& response) override
		{
			//TODO implement
		}


		void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override
		{
			auto it = response->find("Results");
			if(it == response->end() || !it->is_array())
			{
				throw_invalid_response();
			}
			_result = std::make_shared<ResultType>();
			for(const auto& result : *it)
			{
				_result->push_back(result.get<GetResponse>());
			}
		}
	};
}
