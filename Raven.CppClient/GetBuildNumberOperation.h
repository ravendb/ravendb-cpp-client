#pragma once
#include "RavenCommand.h"
#include "IServerOperation.h"
#include "BuildNumber.h"

namespace ravendb::client::serverwide::operations
{
	class GetBuildNumberOperation : public IServerOperation<BuildNumber>
	{
	public:
		~GetBuildNumberOperation() override = default;

		GetBuildNumberOperation() = default;

		std::unique_ptr<http::RavenCommand<BuildNumber>> get_command(
			std::shared_ptr<documents::conventions::DocumentConventions> conventions) const override
		{
			return std::make_unique<GetBuildNumberCommand>();
		}

	private:
		class GetBuildNumberCommand : public http::RavenCommand<BuildNumber>
		{
		public:
			~GetBuildNumberCommand() override = default;

			GetBuildNumberCommand() = default;

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override
			{
				std::ostringstream url_builder;
				url_builder << node->url << "/build/version";

				curl_easy_setopt(curl_ref.get(), CURLOPT_HTTPGET, 1);
				curl_ref.method = constants::methods::GET;

				url_ref.emplace(url_builder.str());
			}

			void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override
			{
				if (!response)
				{
					throw_invalid_response();
				}
				_result = std::make_shared<ResultType>(response->get<ResultType>());
			}

			bool is_read_request() const override
			{
				return true;
			}
		};
	};
}
