#pragma once
#include "stdafx.h"
#include "IMaintenanceOperation.h"
#include "RavenCommand.h"
#include "ServerNode.h"
#include "DatabaseStatistics.h"

namespace ravendb::client::documents::operations
{
	class GetStatisticsOperation : public operations::IMaintenanceOperation<DatabaseStatistics>
	{
	private:
		const std::optional<std::string> _debug_tag{};

	public:
		~GetStatisticsOperation() override = default;

		explicit GetStatisticsOperation(std::optional<std::string> debug_tag = {})
			: _debug_tag(std::move(debug_tag))
		{}

		std::unique_ptr<RavenCommand<DatabaseStatistics>> get_command(std::shared_ptr<DocumentConventions> conventions) const override
		{
			return std::make_unique<GetStatisticsCommand>(_debug_tag);
		}

	private:
		class GetStatisticsCommand : public RavenCommand<DatabaseStatistics>
		{
		private:
			const std::optional<std::string> _debug_tag{};

		public:
			~GetStatisticsCommand() override = default;

			explicit GetStatisticsCommand(std::optional<std::string> debug_tag)
				: _debug_tag(std::move(debug_tag))
			{}

			void create_request(CURL* curl, const http::ServerNode& node, std::string& url) override
			{
				std::ostringstream path_builder;
				path_builder << node.url << "/databases/" << node.database << "/stats";

				if(_debug_tag)
				{
					path_builder << "?" << *_debug_tag;
				}

				curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

				url = path_builder.str();
			}

			void set_response(CURL* curl, const nlohmann::json& response, bool from_cache) override
			{
				if (!response.is_null())
				{
					_result = std::make_shared<ResultType>(response.get<ResultType>());
				}
			}

			bool is_read_request() const noexcept override
			{
				return true;
			}
		};
	};
}
