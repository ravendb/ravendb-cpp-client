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
			_response_type = http::RavenCommandResponseType::RAW;
		}

		void create_request(CURL* curl, const ServerNode& node, std::string& url) override
		{
			std::ostringstream url_builder;
			url_builder << node.url << "/databases/" << node.database;

			_base_url = url_builder.str();

			curl_easy_setopt(curl, CURLOPT_HTTPPOST, 1);

			//TODO use the cache

			nlohmann::json object = nlohmann::json::object();
			nlohmann::json array = nlohmann::json::array();

			using impl::utils::json_utils::set_val_to_json;

			for(const auto& command : _commands.get())
			{
				nlohmann::json temp_object = nlohmann::json::object();

				std::ostringstream temp_url{};
				temp_url << "/databases/" << node.database + command.url;
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
			curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, object.dump().c_str());

			_headers_list.append("Content-Type: application/json");
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, _headers_list.get());

			url_builder << "/multi_get";
			url = url_builder.str();
		}

		bool is_read_request() const noexcept override
		{
			return false;
		}

		void set_response(CURL* curl, const nlohmann::json& response, bool from_cache) override
		{
			auto it = response.find("Results");
			if(it == response.end() || !it->is_array())
			{
				throw std::invalid_argument("wrong response");
			}
			for(const auto& result : *it)
			{
				_result.push_back(result.get<GetResponse>());
			}
		}
	};
}
