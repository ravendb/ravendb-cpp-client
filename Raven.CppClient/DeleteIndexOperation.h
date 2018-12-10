#pragma once
#include "stdafx.h"
#include "IVoidMaintenanceOperation.h"
#include "utils.h"

namespace ravendb::client::documents::operations::indexes
{
	using
		ravendb::client::http::RavenCommand,
		ravendb::client::http::ServerNode,
		ravendb::client::http::VoidRavenCommand;

	class DeleteIndexOperation : public IVoidMaintenanceOperation
	{
	private:
		const std::string _index_name;

	public:
		~DeleteIndexOperation() override = default;

		DeleteIndexOperation(std::string index_name)
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

		std::unique_ptr<VoidRavenCommandBase> get_command(const DocumentConventions& conventions) override
		{
			return std::make_unique<DeleteIndexCommand>(_index_name);
		}

	private:
		class DeleteIndexCommand : public VoidRavenCommand
		{
		private:
			const std::string _index_name;

		public:
			~DeleteIndexCommand() override = default;

			DeleteIndexCommand(std::string index_name)
				: _index_name([&]
			{
				if(index_name.empty())
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
					<< "/indexes?name=" << impl::utils::url_escape(curl, _index_name);

				curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");

				url = pathBuilder.str();
			}
		};
	};
}

