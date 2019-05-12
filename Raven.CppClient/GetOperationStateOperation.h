#pragma once
#include "IMaintenanceOperation.h"

namespace ravendb::client::documents::operations
{
	class GetOperationStateOperation : public operations::IMaintenanceOperation<nlohmann::json>
	{
	private:
		const int64_t _id;
		
	public:
		~GetOperationStateOperation() override = default;

		explicit GetOperationStateOperation(int64_t id)
			: _id(id)
		{}

		std::unique_ptr<http::RavenCommand<nlohmann::json>> get_command(std::shared_ptr<conventions::DocumentConventions> conventions) const override
		{
			return std::make_unique<GetOperationStateCommand>(conventions::DocumentConventions::default_conventions(), _id);
		}

	private:
		class GetOperationStateCommand : public http::RavenCommand<nlohmann::json>
		{
		private:
			const std::shared_ptr<conventions::DocumentConventions> _conventions;
			const int64_t _id;

		public:
			~GetOperationStateCommand() override = default;

			GetOperationStateCommand(std::shared_ptr<conventions::DocumentConventions> conventions, int64_t id)
				: _conventions(conventions)
				, _id(id)
			{}

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override
			{
				std::ostringstream path_builder;
				path_builder << node->url << "/databases/" << node->database
					<< "/operations/state?id=" << _id;

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
				_result = std::make_shared<ResultType>(response->get<ResultType>());
			}

			bool is_read_request() const override
			{
				return true;
			}
		};
	};
}
