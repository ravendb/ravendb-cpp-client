#pragma once
#include "IMaintenanceOperation.h"
#include "IndexDefinition.h"
#include "json_utils.h"

namespace ravendb::client::documents::operations::indexes
{
	class IndexHasChangedOperation : public IMaintenanceOperation<bool>
	{
	private:
		const documents::indexes::IndexDefinition _index_definition;

	public:
		~IndexHasChangedOperation() override = default;

		explicit IndexHasChangedOperation(documents::indexes::IndexDefinition definition)
			: _index_definition(std::move(definition))
		{}

		std::unique_ptr<RavenCommand<bool>> get_command(
			std::shared_ptr<conventions::DocumentConventions> conventions) const override
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

			IndexHasChangedCommand(std::shared_ptr<conventions::DocumentConventions> conventions, const documents::indexes::IndexDefinition& definition)
				: _definition(definition)
			{}

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override
			{
				std::ostringstream path_builder;
				path_builder << node->url << "/databases/" << node->database
					<< "/indexes/has-changed";

				curl_ref.headers.insert_or_assign(constants::headers::CONTENT_TYPE, "application/json");

				curl_easy_setopt(curl_ref.get(), CURLOPT_HTTPPOST, 1);

				auto&& json_str = _definition.dump();

				curl_easy_setopt(curl_ref.get(), CURLOPT_COPYPOSTFIELDS, json_str.c_str());

				curl_ref.method = constants::methods::POST;

				url_ref.emplace(path_builder.str());
			}

			void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override
			{
				if(!response)
				{
					throw_invalid_response();
				}
				_result = std::make_shared<ResultType>();
				if(! impl::utils::json_utils::get_val_from_json(*response, "Changed", *_result))
				{
					throw_invalid_response();
				}
			}

			bool is_read_request() const override
			{
				return false;
			}
		};
	};
}

