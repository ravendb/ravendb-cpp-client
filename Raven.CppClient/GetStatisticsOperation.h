#pragma once
#include "IMaintenanceOperation.h"
#include "DatabaseStatistics.h"

namespace ravendb::client::documents::operations
{
	class GetStatisticsOperation : public IMaintenanceOperation<DatabaseStatistics>
	{
	private:
		const std::optional<std::string> _debug_tag{};

	public:
		~GetStatisticsOperation() override = default;

		explicit GetStatisticsOperation(std::optional<std::string> debug_tag = {})
			: _debug_tag(std::move(debug_tag))
		{}

		std::unique_ptr<http::RavenCommand<DatabaseStatistics>> get_command(
			std::shared_ptr<conventions::DocumentConventions> conventions) const override
		{
			return std::make_unique<GetStatisticsCommand>(_debug_tag);
		}

	private:
		class GetStatisticsCommand : public http::RavenCommand<DatabaseStatistics>
		{
		private:
			const std::optional<std::string> _debug_tag{};

		public:
			~GetStatisticsCommand() override = default;

			explicit GetStatisticsCommand(std::optional<std::string> debug_tag)
				: _debug_tag(std::move(debug_tag))
			{}

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override
			{
				std::ostringstream path_builder;

				path_builder << node->url << "/databases/" << node->database << "/stats";

				if(_debug_tag)
				{
					path_builder << "?" << *_debug_tag;
				}

				curl_easy_setopt(curl_ref.get(), CURLOPT_HTTPGET, 1);
				curl_ref.method = constants::methods::GET;

				url_ref.emplace(path_builder.str());
			}

			void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override
			{
				_result = std::make_shared<ResultType>(response->get<ResultType>());
			}

			bool is_read_request() const noexcept override
			{
				return true;
			}
		};
	};
}
