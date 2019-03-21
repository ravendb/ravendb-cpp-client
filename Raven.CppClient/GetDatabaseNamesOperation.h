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

			void create_request(CURL* curl, const http::ServerNode& node, std::string& url) override;

			void set_response(CURL* curl, const nlohmann::json& response, bool from_cache) override;

			bool is_read_request() const noexcept override;
		};
	};
}
