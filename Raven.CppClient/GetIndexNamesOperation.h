#pragma once
#include "stdafx.h"
#include "IMaintenanceOperation.h"
#include "RavenCommand.h"
#include "json_utils.h"

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

		std::unique_ptr<http::RavenCommand<std::vector<std::string>>> get_command(
			std::shared_ptr<conventions::DocumentConventions> conventions) const override
		{
			return std::make_unique<GetIndexNamesCommand>(_start, _page_size);
		}

	private:
		class GetIndexNamesCommand : public http::RavenCommand<std::vector<std::string>>
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

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override
			{
				std::ostringstream path_builder;
				path_builder << node->url << "/databases/" << node->database
					<< "/indexes?start=" << _start << "&pageSize=" << _page_size
					<< "&namesOnly=true";

				curl_easy_setopt(curl_ref.get(), CURLOPT_HTTPGET, 1);
				curl_ref.method = constants::methods::GET;

				url_ref.emplace(path_builder.str());
			}

			void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override
			{
				if(!response)
				{
					throw_invalid_response();
				}

				_result = std::make_shared<ResultType>();
				if(! impl::utils::json_utils::get_val_from_json(*response, "Results", *_result))
				{
					throw_invalid_response();
				}
			}

			bool is_read_request() const override
			{
				return true;
			}
		};
	};
}
