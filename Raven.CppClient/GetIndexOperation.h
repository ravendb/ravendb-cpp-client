#pragma once
#include "stdafx.h"
#include "IMaintenanceOperation.h"
#include "RavenCommand.h"
#include "ServerNode.h"
#include "IndexDefinition.h"
#include "utils.h"

namespace ravendb::client::documents::operations::indexes
{
	using
		ravendb::client::http::RavenCommand,
		ravendb::client::http::ServerNode,
		ravendb::client::documents::indexes::IndexDefinition;

	class GetIndexOperation : public operations::IMaintenanceOperation<IndexDefinition>
	{
	private:
		const std::string _index_name;

	public:
		~GetIndexOperation() override = default;

		GetIndexOperation (std::string index_name)
			: _index_name(std::move(index_name))
		{}

		std::unique_ptr<RavenCommand<IndexDefinition>> get_command(const DocumentConventions& conventions) override
		{
			return std::unique_ptr<RavenCommand<IndexDefinition>>(
				std::make_unique<GetIndexCommand>(_index_name));
		}

	private:
		class GetIndexCommand : public RavenCommand<IndexDefinition>
		{
		private:
			const std::string _index_name;

		public:
			~GetIndexCommand() override = default;

			GetIndexCommand(std::string index_name)
				: _index_name(std::move(index_name))
			{}

			void create_request(CURL* curl, const ServerNode& node, std::string& url) const override
			{
				std::ostringstream pathBuilder;
				pathBuilder << node.url << "/databases/" << node.database
					<< "/indexes?name=" << ravendb::client::impl::utils::url_escape(curl, _index_name);

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
					_result = (*results_it)[0];
				}
			}

			bool is_read_request() const noexcept override
			{
				return true;
			}
		};
	};
}



