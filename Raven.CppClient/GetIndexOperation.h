#pragma once
#include "stdafx.h"
#include "IMaintenanceOperation.h"
#include "RavenCommand.h"
#include "ServerNode.h"
#include "IndexDefinition.h"
#include "utils.h"
#include "json_utils.h"

using
	ravendb::client::http::RavenCommand,
	ravendb::client::http::ServerNode,
	ravendb::client::documents::indexes::IndexDefinition;

namespace ravendb::client::documents::operations::indexes
{
	class GetIndexOperation : public operations::IMaintenanceOperation<IndexDefinition>
	{
	private:
		const std::string _index_name;

	public:
		~GetIndexOperation() override = default;

		explicit GetIndexOperation (std::string index_name)
			: _index_name(std::move(index_name))
		{}

		std::unique_ptr<RavenCommand<IndexDefinition>> get_command(std::shared_ptr<DocumentConventions> conventions) const override
		{
			return std::make_unique<GetIndexCommand>(_index_name);
		}

	private:
		class GetIndexCommand : public RavenCommand<IndexDefinition>
		{
		private:
			const std::string _index_name;

		public:
			~GetIndexCommand() override = default;

			explicit GetIndexCommand(std::string index_name)
				: _index_name(std::move(index_name))
			{}

			void create_request(CURL* curl, const ServerNode& node, std::string& url) override
			{
				std::ostringstream path_builder;
				path_builder << node.url << "/databases/" << node.database
					<< "/indexes?name=" << ravendb::client::impl::utils::url_escape(curl, _index_name);

				curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

				url = path_builder.str();
			}

			void set_response(CURL* curl, const nlohmann::json& response, bool from_cache) override
			{
				if(std::vector<IndexDefinition> results{};
					! impl::utils::json_utils::get_val_from_json(response, "Results", results) ||
					results.size() != 1 )
				{
					throw ravendb::client::RavenError({}, ravendb::client::RavenError::ErrorType::INVALID_RESPONSE);
				}
				else
				{
					_result = std::make_shared<ResultType>(std::move(results[0]));
				}
			}

			bool is_read_request() const noexcept override
			{
				return true;
			}
		};
	};
}



