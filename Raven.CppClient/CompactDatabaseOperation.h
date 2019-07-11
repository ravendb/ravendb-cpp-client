#pragma once
#include "IServerOperation.h"
#include "OperationIdResult.h"
#include "CompactSettings.h"
#include "RavenCommand.h"

namespace ravendb::client::documents::operations
{
	class CompactDatabaseOperation : public serverwide::operations::IServerOperation<OperationIdResult>
	{
	private:
		const serverwide::CompactSettings _compact_settings;

	public:
		~CompactDatabaseOperation() override = default;

		explicit CompactDatabaseOperation(serverwide::CompactSettings compact_settings)
			: _compact_settings(std::move(compact_settings))
		{}

		std::unique_ptr<http::RavenCommand<OperationIdResult>> get_command
		(std::shared_ptr<conventions::DocumentConventions> conventions) const override
		{
			return std::make_unique<CompactDatabaseCommand>(_compact_settings);
		}

	private:
		class CompactDatabaseCommand : public http::RavenCommand<OperationIdResult>
		{
		private:
			const std::shared_ptr<const http::ServerNode> _node;
			const std::string _compact_settings_json_str;

		public:
			~CompactDatabaseCommand() override = default;

			explicit CompactDatabaseCommand(const serverwide::CompactSettings& compact_settings)
				:_node(compact_settings.node)
				, _compact_settings_json_str(nlohmann::json(compact_settings).dump())
			{}

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override
			{
				//TODO IMPORTANT !!!! Using Specific Node !!!
				node = _node ? _node : node;
				//////////////////////////////////////////

				auto curl = curl_ref.get();
				std::ostringstream path_builder;

				path_builder << node->url << "/admin/compact";

				curl_ref.headers.insert_or_assign("Transfer-Encoding", "chunked");
				curl_ref.headers.insert_or_assign(constants::headers::CONTENT_TYPE, "application/json");
				curl_ref.method = constants::methods::POST;

				curl_easy_setopt(curl, CURLOPT_POST, 1L);
				curl_easy_setopt(curl_ref.get(), CURLOPT_POSTFIELDSIZE_LARGE, curl_off_t(_compact_settings_json_str.size()));
				curl_easy_setopt(curl_ref.get(), CURLOPT_POSTFIELDS, _compact_settings_json_str.c_str());
				
				url_ref.emplace(path_builder.str());
			}

			void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override
			{
				if (!response)
				{
					throw_invalid_response();
				}
				_result = std::make_shared<ResultType>(response->get<ResultType>());
			}

			bool is_read_request() const override
			{
				return false;
			}
		};
	};
}
