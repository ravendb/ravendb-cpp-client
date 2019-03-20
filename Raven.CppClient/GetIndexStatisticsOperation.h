#pragma once
#include "IMaintenanceOperation.h"
#include "RavenCommand.h"
#include "ServerNode.h"
#include "json_utils.h"
#include "IndexStats.h"

namespace ravendb::client::documents::operations::indexes
{
	class GetIndexStatisticsOperation : public operations::IMaintenanceOperation<documents::indexes::IndexStats>
	{
	private:
		const std::string _index_name;

	public:
		~GetIndexStatisticsOperation() override = default;

		explicit GetIndexStatisticsOperation(std::string index_name)
			: _index_name(std::move(index_name))
		{}

		std::unique_ptr<RavenCommand<documents::indexes::IndexStats>> get_command(std::shared_ptr<DocumentConventions> conventions) const override
		{
			return std::make_unique<GetIndexStatisticsCommand>(_index_name);
		}

	private:
		class GetIndexStatisticsCommand : public RavenCommand<documents::indexes::IndexStats>
		{
		private:
			const std::string _index_name;

		public:
			~GetIndexStatisticsCommand() override = default;

			explicit GetIndexStatisticsCommand(std::string index_name)
				: _index_name(std::move(index_name))
			{}

			void create_request(CURL* curl, const http::ServerNode& node, std::string& url) override
			{
				std::ostringstream path_builder;
				path_builder << node.url << "/databases/" << node.database
					<< "/indexes/stats?name=" << impl::utils::url_escape(curl, _index_name);

				curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

				url = path_builder.str();
			}

			void set_response(CURL* curl, const nlohmann::json& response, bool from_cache) override
			{
				std::vector<documents::indexes::IndexStats> stats{};
				if (!impl::utils::json_utils::get_val_from_json(response, "Results", stats) ||
					stats.size() != 1)
				{
					throw ravendb::client::RavenError({}, ravendb::client::RavenError::ErrorType::INVALID_RESPONSE);
				}
				_result = std::make_shared<ResultType>(stats[0]);
			}

			bool is_read_request() const noexcept override
			{
				return true;
			}
		};
	};
}



