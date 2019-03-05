#pragma once
#include "stdafx.h"
#include "IVoidMaintenanceOperation.h"
#include "utils.h"

using
ravendb::client::http::RavenCommand,
ravendb::client::http::ServerNode,
ravendb::client::http::VoidRavenCommand;

namespace ravendb::client::documents::operations::indexes
{
	class DisableIndexOperation : public IVoidMaintenanceOperation
	{
	private:
		const std::string _index_name;

	public:
		~DisableIndexOperation() override = default;

		explicit DisableIndexOperation(std::string index_name)
			: _index_name([&]
		{
			if (index_name.empty())
			{
				throw std::invalid_argument("Index name must have a non empty value");
			}
			else
			{
				return std::move(index_name);
			}
		}())
		{}

		std::unique_ptr<VoidRavenCommandBase> get_command(std::shared_ptr<DocumentConventions> conventions) const override
		{
			return std::make_unique<DisableIndexCommand>(_index_name);
		}

	private:
		class DisableIndexCommand : public VoidRavenCommand
		{
		private:
			const std::string _index_name;

		public:
			~DisableIndexCommand() override = default;

			explicit DisableIndexCommand(std::string index_name)
				: _index_name([&]
			{
				if (index_name.empty())
				{
					throw std::invalid_argument("Index name must have a non empty value");
				}
				else
				{
					return std::move(index_name);
				}
			}())
			{}

			void create_request(CURL* curl, const ServerNode& node, std::string& url) override
			{
				std::ostringstream pathBuilder;
				pathBuilder << node.url << "/databases/" << node.database
					<< "/admin/indexes/disable?name=" << impl::utils::url_escape(curl, _index_name);

				curl_easy_setopt(curl, CURLOPT_HTTPPOST, 1);
				curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, "");

				url = pathBuilder.str();
			}
		};
	};
}

