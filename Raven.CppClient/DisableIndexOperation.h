#pragma once
#include "IVoidMaintenanceOperation.h"

namespace ravendb::client::documents::operations::indexes
{
	class DisableIndexOperation : public IVoidMaintenanceOperation
	{
	private:
		const std::string _index_name;

	public:
		~DisableIndexOperation() override = default;

		explicit DisableIndexOperation(std::string index_name);

		std::unique_ptr<http::VoidRavenCommandBase> get_command(
			std::shared_ptr<conventions::DocumentConventions> conventions) const override;

	private:
		class DisableIndexCommand : public http::VoidRavenCommand
		{
		private:
			const std::string _index_name;

		public:
			~DisableIndexCommand() override = default;

			explicit DisableIndexCommand(std::string index_name);

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override;
		};
	};
}

