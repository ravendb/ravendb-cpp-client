#pragma once
#include "stdafx.h"
#include "IMaintenanceOperation.h"
#include "RavenCommand.h"
#include "PutIndexResult.h"
#include "IndexDefinition.h"

namespace ravendb::client::documents::operations::indexes
{
	class PutIndexesOperation : public IMaintenanceOperation<std::vector<documents::indexes::PutIndexResult>>
	{
	private:
		const std::vector<documents::indexes::IndexDefinition> _index_to_add;

	public:
		~PutIndexesOperation() override = default;

		explicit PutIndexesOperation(std::vector<documents::indexes::IndexDefinition> index_to_add)
			: _index_to_add([&]()
			{
				if(index_to_add.size() == 0)
				{
					throw std::runtime_error("index_to_add cannot be empty");
				}
				else
				{
					return std::move(index_to_add);
				}
			}())
		{}
		
		std::unique_ptr<http::RavenCommand<std::vector<documents::indexes::PutIndexResult>>> get_command(
			std::shared_ptr<conventions::DocumentConventions> conventions) const override
		{
			return std::make_unique<PutIndexesCommand>(_index_to_add);
		}

	private:
		class PutIndexesCommand : public http::RavenCommand<std::vector<documents::indexes::PutIndexResult>>
		{
		private:
			const std::vector<nlohmann::json> _index_to_add;
			std::istringstream _index_to_add_stream;

		public:
			~PutIndexesCommand() override = default;

			explicit PutIndexesCommand(const std::vector<documents::indexes::IndexDefinition>& indexes_to_add)
				: _index_to_add([&]
				{
					if (indexes_to_add.size() == 0)
					{
						throw std::runtime_error("indexes_to_add cannot be empty");
					}
					std::remove_const_t<decltype(_index_to_add)> temp_index_to_add{};
					for (const auto& index : indexes_to_add)
					{
						nlohmann::json index_json = index;
						temp_index_to_add.emplace_back(std::move(index_json));
					}
					return temp_index_to_add;
				}())
				, _index_to_add_stream([this]()
				{
						nlohmann::json json_to_put;
						json_to_put.emplace("Indexes",_index_to_add);
						return json_to_put.dump();
				}())
			{}

			void create_request(impl::CurlHandlesHolder::CurlReference& curl_ref, std::shared_ptr<const http::ServerNode> node,
				std::optional<std::string>& url_ref) override
			{
				auto curl = curl_ref.get();
				std::ostringstream path_builder;

				path_builder << node->url << "/databases/" << node->database
					<< "/admin/indexes";

				curl_ref.method = constants::methods::PUT;
				curl_ref.headers.insert_or_assign("Transfer-Encoding", "chunked");
				curl_ref.headers.emplace(constants::headers::CONTENT_TYPE, "application/json");

				curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
				curl_easy_setopt(curl, CURLOPT_READFUNCTION, stream_read_callback);
				curl_easy_setopt(curl, CURLOPT_READDATA, &_index_to_add_stream);

				_index_to_add_stream.clear();
				_index_to_add_stream.seekg(0);

				url_ref.emplace(path_builder.str());
			}

			void set_response(const std::optional<nlohmann::json>& response, bool from_cache) override
			{
				_result = std::make_shared<ResultType>();
				impl::utils::json_utils::get_val_from_json(*response, "Results", *_result);
			}

			bool is_read_request() const override
			{
				return false;
			}
		};
	};
}



