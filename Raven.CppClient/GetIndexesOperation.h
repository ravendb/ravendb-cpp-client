#pragma once
#include "stdafx.h"
#include "IMaintenanceOperation.h"
#include "RavenCommand.h"
#include "ServerNode.h"
#include "IndexDefinition.h"

using
	ravendb::client::http::RavenCommand,
	ravendb::client::http::ServerNode,
	ravendb::client::documents::indexes::IndexDefinition;

namespace ravendb::client::documents::operations::indexes
{
	class GetIndexesOperation : public operations::IMaintenanceOperation<std::vector<IndexDefinition>>
	{
	private:
		const int32_t _start;
		const int32_t _page_size;

	public:
		~GetIndexesOperation() override = default;

		GetIndexesOperation(int32_t start, int32_t page_size)
			: _start(start)
			, _page_size(page_size)
		{}

		std::unique_ptr<RavenCommand<std::vector<IndexDefinition>>> get_command(
			std::shared_ptr<conventions::DocumentConventions> conventions) const override
		{
			return std::make_unique<GetIndexesCommand>(_start, _page_size);
		}

	private:
		class GetIndexesCommand : public RavenCommand<std::vector<IndexDefinition>>
		{
		private:
			const int32_t _start;
			const int32_t _page_size;

		public:
			~GetIndexesCommand() override = default;

			GetIndexesCommand(int32_t start, int32_t page_size)
				: _start(start)
				, _page_size(page_size)
			{}

			void create_request(CURL* curl, const ServerNode& node, std::string& url) override
			{
				std::ostringstream path_builder;
				path_builder << node.url << "/databases/" << node.database
					<< "/indexes?start=" << _start << "&pageSize=" << _page_size;

				curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

				url = path_builder.str();
			}

			void set_response(CURL* curl, const nlohmann::json& response, bool from_cache) override
			{
				_result = std::make_shared<ResultType>();
				if (!impl::utils::json_utils::get_val_from_json(response, "Results", *_result))
				{
					throw ravendb::client::RavenError({}, ravendb::client::RavenError::ErrorType::INVALID_RESPONSE);
				}
			}

			bool is_read_request() const noexcept override
			{
				return true;
			}
		};
	};
}



