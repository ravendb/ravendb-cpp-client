#pragma once
#include "stdafx.h"
#include "RavenCommand.h"
#include "json_utils.h"
#include "IServerOperation.h"

namespace ravendb::client::serverwide::operations
{
	namespace get_db_names_op
	{
		struct DatabaseNames //used with from_json
		{
			std::vector<std::string> value;
		};

		inline void from_json(const nlohmann::json& j, DatabaseNames& result)
		{
			using ravendb::client::impl::utils::json_utils::get_val_from_json;

			get_val_from_json(j, "Databases", result.value);
		}
	}

	class GetDatabaseNamesOperation : public IServerOperation<std::vector<std::string>>
	{
	private:
		const int32_t _start;
		const int32_t _page_size;

	public:
		~GetDatabaseNamesOperation() override = default;

		GetDatabaseNamesOperation(int32_t _start, int32_t _page_size)
			: _start(_start)
			, _page_size(_page_size)
		{}

		std::unique_ptr<RavenCommand<std::vector<std::string>>> get_command(const DocumentConventions& conventions) override
		{
			return std::make_unique<GetDatabaseNamesCommand>(_start, _page_size);
		}

	private:
		class GetDatabaseNamesCommand : public RavenCommand<std::vector<std::string>>
		{
		private:
			const int32_t _start;
			const int32_t _page_size;

		public:

			~GetDatabaseNamesCommand() override = default;

			GetDatabaseNamesCommand(int32_t _start, int32_t _page_size)
				: _start(_start)
				, _page_size(_page_size)
			{}

			void create_request(CURL* curl, const ServerNode& node, std::string& url) override
			{
				std::ostringstream pathBuilder;
				pathBuilder << node.url << "/databases?start=" << _start
					<< "&pageSize=" << _page_size << "&namesOnly=true";

				curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

				url = pathBuilder.str();
			}


			void set_response(CURL* curl, const nlohmann::json& response, bool from_cache) override
			{
				auto it = response.find("Databases");
				if(it == response.end() || ! it->is_array())
				{
					throw ravendb::client::RavenError({},ravendb::client::RavenError::ErrorType::invalid_response);
				}
				get_db_names_op::DatabaseNames res = response;
				_result = std::move(res.value);
			}

			bool is_read_request() const noexcept override
			{
				return true;
			}
		};
	};
}
