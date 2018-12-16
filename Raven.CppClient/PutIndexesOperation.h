#pragma once
#include "stdafx.h"
#include "IMaintenanceOperation.h"
#include "RavenCommand.h"
#include "ServerNode.h"
#include "PutIndexResult.h"
#include "IndexDefinition.h"
#include "utils.h"

using
	ravendb::client::http::RavenCommand,
	ravendb::client::http::ServerNode,
	ravendb::client::documents::indexes::IndexDefinition,
	ravendb::client::documents::indexes::PutIndexResult;

namespace ravendb::client::documents::operations::indexes
{
	class PutIndexesOperation : public IMaintenanceOperation<std::vector<PutIndexResult>>
	{
	private:
		const std::vector<IndexDefinition> _index_to_add;

	public:
		~PutIndexesOperation() override = default;

		explicit PutIndexesOperation(std::vector<IndexDefinition> index_to_add)
			: _index_to_add([&]()
			{
				if(index_to_add.size() == 0)
				{
					throw std::runtime_error("index_to_add cannot be empty");
				}
				else
				{
					return std::move(index_to_add);
				}
			}())
		{}
		
		std::unique_ptr<RavenCommand<std::vector<PutIndexResult>>> get_command(const DocumentConventions& conventions) const override
		{
			return std::make_unique<PutIndexesCommand>(_index_to_add);
		}

	private:
		class PutIndexesCommand : public RavenCommand<std::vector<PutIndexResult>>
		{
		private:
			const std::vector<nlohmann::json> _index_to_add;
			const std::string _index_to_add_str;

		public:
			~PutIndexesCommand() override = default;

			explicit PutIndexesCommand(const std::vector<IndexDefinition>& indexes_to_add)
				: _index_to_add([&]
				{
					if (indexes_to_add.size() == 0)
					{
						throw std::runtime_error("indexes_to_add cannot be empty");
					}
					std::remove_const_t<decltype(_index_to_add)> temp_index_to_add{};
					for (const auto& index : indexes_to_add)
					{
						nlohmann::json index_json = index;
						temp_index_to_add.emplace_back(std::move(index_json));
					}
					return std::move(temp_index_to_add);
				}())
				, _index_to_add_str([this]
				{
						nlohmann::json json_to_put;
						json_to_put.emplace("Indexes",_index_to_add);
						return json_to_put.dump();
				}())
			{}

			void create_request(CURL* curl, const ServerNode& node, std::string& url) override
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
				if (!impl::utils::json_utils::get_val_from_json(response, "Results", _result))
				{
					throw ravendb::client::RavenError({}, ravendb::client::RavenError::ErrorType::invalid_response);
				}
			}

			bool is_read_request() const noexcept override
			{
				return false;
			}
		};
	};
}



