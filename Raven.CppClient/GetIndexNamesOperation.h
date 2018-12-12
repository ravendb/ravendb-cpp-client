#pragma once
#include "stdafx.h"
#include "IMaintenanceOperation.h"
#include "RavenCommand.h"
#include "ServerNode.h"
#include "json_utils.h"

using
ravendb::client::http::RavenCommand,
ravendb::client::http::ServerNode;


namespace ravendb::client::documents::operations::indexes
{
	class GetIndexNamesOperation : public operations::IMaintenanceOperation<std::vector<std::string>>
	{
	private:
		const int32_t _start;
		const int32_t _page_size;

	public:
		~GetIndexNamesOperation() override = default;

		GetIndexNamesOperation(int32_t start, int32_t page_size)
			: _start(start)
			, _page_size(page_size)
		{}

		std::unique_ptr<RavenCommand<std::vector<std::string>>> get_command(const DocumentConventions& conventions) override
		{
			return std::make_unique<GetIndexNamesCommand>(_start, _page_size);
		}

	private:
		class GetIndexNamesCommand : public RavenCommand<std::vector<std::string>>
		{
		private:
			const int32_t _start;
			const int32_t _page_size;

		public:
			~GetIndexNamesCommand() override = default;

			GetIndexNamesCommand(int32_t start, int32_t page_size)
				: _start(start)
				, _page_size(page_size)
			{}

			void create_request(CURL* curl, const ServerNode& node, std::string& url) override
			{
				std::ostringstream pathBuilder;
				pathBuilder << node.url << "/databases/" << node.database
					<< "/indexes?start=" << _start << "&pageSize=" << _page_size
					<< "&namesOnly=true";

				curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

				url = pathBuilder.str();
			}

			void set_response(CURL* curl, const nlohmann::json& response, bool from_cache) override
			{
				if(! impl::utils::json_utils::get_val_from_json(response, "Results", _result))
				{
					throw ravendb::client::RavenError({}, ravendb::client::RavenError::ErrorType::invalid_response);
				}
			}

			bool is_read_request() const noexcept override
			{
				return true;
			}
		};
	};
}
