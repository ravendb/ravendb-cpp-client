#pragma once
#include "IServerOperation.h"
#include "OperationIdResult.h"
#include "CompactSettings.h"

namespace ravendb::client::documents::operations
{
	class CompactDatabaseOperation : public serverwide::operations::IServerOperation<OperationIdResult>
	{
	private:
		const serverwide::CompactSettings _compact_settings;

	public:
		~CompactDatabaseOperation() override = default;

		explicit CompactDatabaseOperation(serverwide::CompactSettings compact_settings);

		std::unique_ptr<http::RavenCommand<OperationIdResult>> get_command(
			std::shared_ptr<conventions::DocumentConventions> conventions) const override;

	private:
		class CompactDatabaseCommand : public http::RavenCommand<OperationIdResult>
		{
		private:
			const std::shared_ptr<const http::ServerNode> _node;
			const std::string _compact_settings_json_str;

		public:
			~CompactDatabaseCommand() override = default;

			explicit CompactDatabaseCommand(const serverwide::CompactSettings& compact_settings);

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override;

			void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override;

			bool is_read_request() const override;
		};
	};
}
