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

		explicit StartIndexOperation(std::string index_name)
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

		std::unique_ptr<http::VoidRavenCommandBase> get_command(
			std::shared_ptr<conventions::DocumentConventions> conventions) const override
		{
			return std::make_unique<StartIndexCommand>(_index_name);
		}

	private:
		class StartIndexCommand : public http::VoidRavenCommand
		{
		private:
			const std::string _index_name;

		public:
			~StartIndexCommand() override = default;

			explicit StartIndexCommand(std::string index_name)
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

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override
			{
				std::ostringstream path_builder;
				path_builder << node->url << "/databases/" << node->database
					<< "/admin/indexes/start?name=" << http::url_encode(curl_ref, _index_name);

				curl_easy_setopt(curl_ref.get(), CURLOPT_POST, 1);
				curl_easy_setopt(curl_ref.get(), CURLOPT_POSTFIELDSIZE, 0);
				curl_ref.method = constants::methods::POST;

				url_ref.emplace(path_builder.str());
			}
		};
	};
}

