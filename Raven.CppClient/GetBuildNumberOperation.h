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
			std::shared_ptr<documents::conventions::DocumentConventions> conventions) const override;

	private:
		class GetBuildNumberCommand : public http::RavenCommand<BuildNumber>
		{
		public:
			~GetBuildNumberCommand() override = default;

			GetBuildNumberCommand() = default;

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override;

			void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override;

			bool is_read_request() const override;
		};
	};
}
