#pragma once
#include "stdafx.h"
#include "IMaintenanceOperation.h"
#include "RavenCommand.h"
#include "ServerNode.h"
#include "IndexDefinition.h"
#include "json_utils.h"

using
	ravendb::client::http::RavenCommand,
	ravendb::client::http::ServerNode,
	ravendb::client::documents::indexes::IndexDefinition;

namespace ravendb::client::documents::operations::indexes
{
	class IndexHasChangedOperation : public IMaintenanceOperation<bool>
	{
	private:
		const IndexDefinition _index_definition;

	public:
		~IndexHasChangedOperation() override = default;

		explicit IndexHasChangedOperation(IndexDefinition definition)
			: _index_definition(std::move(definition))
		{}

		std::unique_ptr<RavenCommand<bool>> get_command(std::shared_ptr<DocumentConventions> conventions) const override
		{
			return std::make_unique<IndexHasChangedCommand>(conventions, _index_definition);
		}

	private:
		class IndexHasChangedCommand : public RavenCommand<bool>
		{
		private:
			nlohmann::json _definition;

		public:
			~IndexHasChangedCommand() override = default;

			IndexHasChangedCommand(std::shared_ptr<DocumentConventions> conventions, const IndexDefinition& definition)
				: _definition(definition)
			{}

			void create_request(CURL* curl, const ServerNode& node, std::string& url) override
			{
				std::ostringstream pathBuilder;
				pathBuilder << node.url << "/databases/" << node.database
					<< "/indexes/has-changed";

				curl_easy_setopt(curl, CURLOPT_HTTPPOST, 1);

				auto&& json_str = _definition.dump();

				curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, json_str.c_str());

				url = pathBuilder.str();
			}

			void set_response(CURL* curl, const nlohmann::json& response, bool from_cache) override
			{
				if(! impl::utils::json_utils::get_val_from_json(response, "Changed", _result))
				{
					throw ravendb::client::RavenError({}, ravendb::client::RavenError::ErrorType::INVALID_RESPONSE);
				}
			}

			bool is_read_request() const noexcept override
			{
				return false;
			}
		};
	};
}

