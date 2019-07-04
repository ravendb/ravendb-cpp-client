#pragma once
#include "IServerOperation.h"
#include "RavenCommand.h"

namespace ravendb::client
{
	namespace http
	{
		struct ServerNode;
	}
}

namespace ravendb::client::serverwide::operations
{
	class GetServerWideOperationStateOperation : IServerOperation<nlohmann::json>
	{
	private:
		const int64_t _id;

	public:
		~GetServerWideOperationStateOperation() override = default;

		explicit GetServerWideOperationStateOperation(int64_t id);

		std::unique_ptr<http::RavenCommand<nlohmann::json>> get_command(
			std::shared_ptr<documents::conventions::DocumentConventions> conventions) const override;

	public:
		class GetServerWideOperationStateCommand : public http::RavenCommand<nlohmann::json>
		{
		private:
			const int64_t _id;
			const std::shared_ptr<documents::conventions::DocumentConventions> _conventions;

		public:
			~GetServerWideOperationStateCommand() override = default;

			GetServerWideOperationStateCommand(std::shared_ptr<documents::conventions::DocumentConventions> conventions,
				int64_t id);

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override;

			void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override;

			bool is_read_request() const override;
		};
	};
}
