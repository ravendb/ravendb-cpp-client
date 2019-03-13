#pragma once
#include "stdafx.h"
#include "IMaintenanceOperation.h"
#include "RavenCommand.h"
#include "ServerNode.h"
#include "IndexingStatus.h"

using
ravendb::client::http::RavenCommand,
ravendb::client::http::ServerNode,
ravendb::client::documents::indexes::IndexingStatus;

namespace ravendb::client::documents::operations::indexes
{
	class GetIndexingStatusOperation : public operations::IMaintenanceOperation<IndexingStatus>
	{
	public:
		~GetIndexingStatusOperation() override = default;

		GetIndexingStatusOperation() = default;

		std::unique_ptr<RavenCommand<IndexingStatus>> get_command(std::shared_ptr<DocumentConventions> conventions) const override
		{
			return std::make_unique<GetIndexingStatusCommand>();
		}

	private:
		class GetIndexingStatusCommand : public RavenCommand<IndexingStatus>
		{
		public:
			~GetIndexingStatusCommand() override = default;

			GetIndexingStatusCommand() = default;

			void create_request(CURL* curl, const ServerNode& node, std::string& url) override
			{
				std::ostringstream path_builder;
				path_builder << node.url << "/databases/" << node.database
					<< "/indexes/status";

				curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

				url = path_builder.str();
			}

			void set_response(CURL* curl, const nlohmann::json& response, bool from_cache) override
			{
				_result = response;
			}

			bool is_read_request() const noexcept override
			{
				return true;
			}
		};
	};
}



