#pragma once
#include "IMaintenanceOperation.h"
#include "IndexDefinition.h"
#include "json_utils.h"

namespace ravendb::client::documents::operations::indexes
{
	class GetIndexOperation : public operations::IMaintenanceOperation<documents::indexes::IndexDefinition>
	{
	private:
		const std::string _index_name;

	public:
		~GetIndexOperation() override = default;

		explicit GetIndexOperation (std::string index_name)
			: _index_name(std::move(index_name))
		{}

		std::unique_ptr<http::RavenCommand<documents::indexes::IndexDefinition>> get_command(
			std::shared_ptr<conventions::DocumentConventions> conventions) const override
		{
			return std::make_unique<GetIndexCommand>(_index_name);
		}

	private:
		class GetIndexCommand : public http::RavenCommand<documents::indexes::IndexDefinition>
		{
		private:
			const std::string _index_name;

		public:
			~GetIndexCommand() override = default;

			explicit GetIndexCommand(std::string index_name)
				: _index_name(std::move(index_name))
			{}

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override
			{
				std::ostringstream path_builder;
				path_builder << node->url << "/databases/" << node->database
					<< "/indexes?name=" << http::url_encode(curl_ref, _index_name);

				curl_easy_setopt(curl_ref.get(), CURLOPT_HTTPGET, 1);
				curl_ref.method = constants::methods::GET;

				url_ref.emplace(path_builder.str());
			}

			void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override
			{
				if(!response)
				{
					return;
				}
				if(std::vector<documents::indexes::IndexDefinition> results{};
					! impl::utils::json_utils::get_val_from_json(*response, "Results", results) ||
					results.size() != 1 )
				{
					throw_invalid_response();
				}
				else
				{
					_result = std::make_shared<ResultType>(std::move(results[0]));
				}
			}

			bool is_read_request() const override
			{
				return true;
			}
		};
	};
}



