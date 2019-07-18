#pragma once
#include "IVoidMaintenanceOperation.h"

namespace ravendb::client::documents::operations::indexes
{
	class StartIndexOperation : public IVoidMaintenanceOperation
	{
	private:
		const std::string _index_name;

	public:
		~StartIndexOperation() override = default;

		explicit StartIndexOperation(std::string index_name);

		std::unique_ptr<http::VoidRavenCommandBase> get_command(
			std::shared_ptr<conventions::DocumentConventions> conventions) const override;

	private:
		class StartIndexCommand : public http::VoidRavenCommand
		{
		private:
			const std::string _index_name;

		public:
			~StartIndexCommand() override = default;

			explicit StartIndexCommand(std::string index_name);

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override;
		};
	};
}

