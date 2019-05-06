#pragma once
#include "IMaintenanceOperation.h"
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

		std::unique_ptr<http::RavenCommand<documents::indexes::IndexStats>> get_command(std::shared_ptr<conventions::DocumentConventions> conventions) const override
		{
			return std::make_unique<GetIndexStatisticsCommand>(_index_name);
		}

	private:
		class GetIndexStatisticsCommand : public http::RavenCommand<documents::indexes::IndexStats>
		{
		private:
			const std::string _index_name;

		public:
			~GetIndexStatisticsCommand() override = default;

			explicit GetIndexStatisticsCommand(std::string index_name)
				: _index_name(std::move(index_name))
			{}

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override
			{
				std::ostringstream path_builder;
				path_builder << node->url << "/databases/" << node->database
					<< "/indexes/stats?name=" << http::url_encode(curl_ref, _index_name);

				curl_easy_setopt(curl_ref.get(), CURLOPT_HTTPGET, 1);
				curl_ref.method = constants::methods::GET;

				url_ref.emplace(path_builder.str());
			}

			void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override
			{
				if(!response)
				{
					throw_invalid_response();
				}

				std::vector<documents::indexes::IndexStats> stats{};
				if (!impl::utils::json_utils::get_val_from_json(*response, "Results", stats) ||
					stats.size() != 1)
				{
					throw_invalid_response();
				}
				_result = std::make_shared<ResultType>(stats[0]);
			}

			bool is_read_request() const override
			{
				return true;
			}
		};
	};
}
