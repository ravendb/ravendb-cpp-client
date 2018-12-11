#pragma once
#include "stdafx.h"
#include "IMaintenanceOperation.h"
#include "RavenCommand.h"
#include "ServerNode.h"
#include "IndexErrors.h"

using
ravendb::client::http::RavenCommand,
ravendb::client::http::ServerNode;

namespace ravendb::client::documents::operations::indexes
{
	class GetIndexErrorsOperation : public operations::IMaintenanceOperation<std::vector<IndexErrors>>
	{
	private:
		const std::vector<std::string> _index_names{};

	public:
		~GetIndexErrorsOperation() override = default;

		GetIndexErrorsOperation() = default;

		GetIndexErrorsOperation(std::vector<std::string> index_names)
			: _index_names(std::move(index_names))
		{}

		std::unique_ptr<RavenCommand<std::vector<IndexErrors>>> get_command(const DocumentConventions& conventions) override
		{
			return std::make_unique<GetIndexErrorsCommand>(_index_names);
		}

	private:
		class GetIndexErrorsCommand : public RavenCommand<std::vector<IndexErrors>>
		{
		private:
			const std::vector<std::string> _index_names{};

		public:
			~GetIndexErrorsCommand() override = default;

			GetIndexErrorsCommand(std::vector<std::string> index_names)
				: _index_names(std::move(index_names))
			{}

			void create_request(CURL* curl, const ServerNode& node, std::string& url) override
			{
				std::ostringstream pathBuilder;
				pathBuilder << node.url << "/databases/" << node.database << "/indexes/errors";

				if(!_index_names.empty())
				{
					pathBuilder << "?";
					for(const auto& index_name : _index_names)
					{
						pathBuilder << "&name=" << index_name;
					}
				}

				curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

				url = pathBuilder.str();
			}

			void set_response(CURL* curl, const nlohmann::json& response, bool from_cache) override
			{
				if (auto results_it = response.find("Results"); results_it == response.end() || !results_it->is_array())
				{
					throw ravendb::client::RavenError({}, ravendb::client::RavenError::ErrorType::invalid_response);
				}
				else
				{
					_result.reserve(results_it->size());
					for (const auto& res : *results_it)
					{
						IndexErrors&& put_res = res;
						_result.emplace_back(std::move(put_res));
					}
				}
			}

			bool is_read_request() const noexcept override
			{
				return true;
			}
		};
	};
}



