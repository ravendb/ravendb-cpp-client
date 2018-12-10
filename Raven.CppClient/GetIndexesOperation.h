#pragma once
#include "stdafx.h"
#include "IMaintenanceOperation.h"
#include "RavenCommand.h"
#include "ServerNode.h"
#include "IndexDefinition.h"

namespace ravendb::client::documents::operations::indexes
{
	using
		ravendb::client::http::RavenCommand,
		ravendb::client::http::ServerNode,
		ravendb::client::documents::indexes::IndexDefinition;

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

		std::unique_ptr<RavenCommand<std::vector<IndexDefinition>>> get_command(const DocumentConventions& conventions) override
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
				std::ostringstream pathBuilder;
				pathBuilder << node.url << "/databases/" << node.database
					<< "/indexes?start=" << _start << "&pageSize=" << _page_size;

				curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

				url = pathBuilder.str();
			}

			void set_response(CURL* curl, const nlohmann::json& response, bool from_cache) override
			{
				if (auto results_it = response.find("Results"); results_it == response.end() || !results_it->is_array())
				{
					throw ravendb::client::RavenError({}, ravendb::client::RavenError::ErrorType::invalid_response);
				}
				else
				{
					_result.reserve(results_it->size());
					for (const auto& res : *results_it)
					{
						IndexDefinition&& put_res = res;
						_result.emplace_back(std::move(put_res));
					}
				}
			}

			bool is_read_request() const noexcept override
			{
				return true;
			}
		};
	};
}



