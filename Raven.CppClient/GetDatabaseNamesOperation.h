#pragma once
#include "RavenCommand.h"
#include "IServerOperation.h"

namespace ravendb::client::serverwide::operations
{
	class GetDatabaseNamesOperation : public IServerOperation<std::vector<std::string>>
	{
	private:
		const int32_t _start;
		const int32_t _page_size;

	public:
		~GetDatabaseNamesOperation() override;

		GetDatabaseNamesOperation(int32_t _start, int32_t _page_size);

		std::unique_ptr<http::RavenCommand<std::vector<std::string>>> get_command(
			std::shared_ptr<documents::conventions::DocumentConventions> conventions) override;

	private:
		class GetDatabaseNamesCommand : public http::RavenCommand<std::vector<std::string>>
		{
		private:
			const int32_t _start;
			const int32_t _page_size;

		public:
			~GetDatabaseNamesCommand() override;

			GetDatabaseNamesCommand(int32_t _start, int32_t _page_size);

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override;

			void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override;

			bool is_read_request() const override;
		};
	};
}
