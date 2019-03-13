#pragma once
#include "stdafx.h"
#include "IMaintenanceOperation.h"
#include "RavenCommand.h"
#include "ServerNode.h"
#include "IndexErrors.h"

namespace ravendb::client::documents::operations::indexes
{
	class GetIndexErrorsOperation : public operations::IMaintenanceOperation<std::vector<documents::indexes::IndexErrors>>
	{
	private:
		const std::vector<std::string> _index_names{};

	public:
		~GetIndexErrorsOperation() override = default;

		GetIndexErrorsOperation() = default;

		GetIndexErrorsOperation(std::vector<std::string> index_names)
			: _index_names(std::move(index_names))
		{}

		std::unique_ptr<RavenCommand<std::vector<documents::indexes::IndexErrors>>> get_command(std::shared_ptr<DocumentConventions> conventions) const override
		{
			return std::make_unique<GetIndexErrorsCommand>(_index_names);
		}

	private:
		class GetIndexErrorsCommand : public RavenCommand<std::vector<documents::indexes::IndexErrors>>
		{
		private:
			const std::vector<std::string> _index_names{};

		public:
			~GetIndexErrorsCommand() override = default;

			GetIndexErrorsCommand(std::vector<std::string> index_names)
				: _index_names(std::move(index_names))
			{}

			void create_request(CURL* curl, const ServerNode& node, std::string& url) override
			{
				std::ostringstream path_builder;
				path_builder << node.url << "/databases/" << node.database << "/indexes/errors";

				if(!_index_names.empty())
				{
					path_builder << "?";
					for(const auto& index_name : _index_names)
					{
						path_builder << "&name=" << index_name;
					}
				}

				curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

				url = path_builder.str();
			}

			void set_response(CURL* curl, const nlohmann::json& response, bool from_cache) override
			{
				if(! impl::utils::json_utils::get_val_from_json(response,"Results",_result))
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



