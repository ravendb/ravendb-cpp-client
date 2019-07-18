#pragma once
#include "IVoidMaintenanceOperation.h"

namespace ravendb::client::documents::operations::indexes
{
	class ResetIndexOperation : public IVoidMaintenanceOperation
	{
	private:
		const std::string _index_name;

	public:
		~ResetIndexOperation() override = default;

		explicit ResetIndexOperation(std::string index_name);

		std::unique_ptr<http::VoidRavenCommandBase> get_command(std::shared_ptr<conventions::DocumentConventions> conventions) const override;

	private:
		class ResetIndexCommand : public http::VoidRavenCommand
		{
		private:
			const std::string _index_name;

		public:
			~ResetIndexCommand() override = default;

			explicit ResetIndexCommand(std::string index_name);

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override;
		};
	};
}

