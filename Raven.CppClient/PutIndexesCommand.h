#pragma once
#include "stdafx.h"
#include "RavenCommand.h"
#include "ServerNode.h"
#include "PutIndexResult.h"
#include "IndexDefinition.h"
#include "utils.h"

namespace ravendb::client::documents::operations::indexes
{
	using	ravendb::client::http::RavenCommand,
		ravendb::client::http::ServerNode,
		ravendb::client::documents::indexes::IndexDefinition,
		ravendb::client::documents::indexes::PutIndexResult;

	class PutIndexesCommand : public RavenCommand<std::vector<PutIndexResult>>
	{
	private:
		std::vector<nlohmann::json> _index_to_add{};
		std::string _index_to_add_str{};

	public:

		~PutIndexesCommand() override = default;

		PutIndexesCommand(const std::vector<IndexDefinition>& indexes_to_add)
		{
			for(const auto& index : indexes_to_add)
			{
				nlohmann::json index_json = index;
				_index_to_add.emplace_back(index_json);

				_index_to_add_str = nlohmann::json(_index_to_add).dump();

			}
		}

		void create_request(CURL* curl, const ServerNode& node, std::string& url) const override
		{
			std::ostringstream pathBuilder;
			pathBuilder << node.url << "/databases/" << node.database
			<< "/admin/indexes";

			

			curl_easy_setopt(curl, CURLOPT_READFUNCTION, ravendb::client::impl::utils::read_callback);
			curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
			curl_easy_setopt(curl, CURLOPT_PUT, 1L);
			curl_easy_setopt(curl, CURLOPT_READDATA, &_index_to_add_str);
			curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)_index_to_add_str.length());

			url = pathBuilder.str();
		}

		void set_response(CURL* curl, const nlohmann::json& response, bool from_cache) override
		{
			_result.reserve(response.size());
			for(const auto& res : response)
			{
				PutIndexResult put_res = res;
				_result.emplace_back(std::move(put_res));
			}
		}

		bool is_read_request() const noexcept override
		{
			return false;
		}
	};
}


