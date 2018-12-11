#pragma once
#include <curl/curl.h>
#include <filesystem>

#include "RequestExecutor.h"
#include "CreateDatabaseOperation.h"
#include "DeleteDatabaseCommand.h"

namespace ravendb::client::tests
{
#ifndef GET_REQUEST_EXECUTOR
#define GET_REQUEST_EXECUTOR() RequestExecutorScope::get_request_executor_with_db(__FILE__, __LINE__, __COUNTER__)
#endif

	inline constexpr char RAVEN_SERVER_URL[] = "http://localhost:8080";

	//using fiddler + verbose
	void set_for_fiddler(CURL* curl);

	//request _executor only - no DB is created
	std::unique_ptr<ravendb::client::http::RequestExecutor> get_raw_request_executor(const std::string& db = {});


	class RequestExecutorScope
	{
	private:
		std::string _db_name;
		std::unique_ptr<ravendb::client::http::RequestExecutor> _executor;
	public:
		RequestExecutorScope(std::string db_name)
			: _db_name(std::move(db_name))
		{
			auto server_wide_req_exec = get_raw_request_executor();

			ravendb::client::serverwide::DatabaseRecord rec{};
			rec.database_name = _db_name;
			serverwide::operations::CreateDatabaseOperation op(rec);
			auto cmd = op.get_command({});
			server_wide_req_exec->execute(*cmd);

			_executor = get_raw_request_executor(_db_name);
		}

		~RequestExecutorScope()
		{
			ravendb::client::serverwide::operations::DeleteDatabaseCommand cmd(_db_name, true, {}, std::chrono::seconds(10));
			_executor->execute<ravendb::client::serverwide::operations::DeleteDatabaseResult>(cmd);
		}

		ravendb::client::http::RequestExecutor* get() const noexcept
		{
			return _executor.get();
		}

		const std::string& get_db_name() const
		{
			return _db_name;
		}

		static std::unique_ptr<RequestExecutorScope> get_request_executor_with_db(const std::string& file, int line, int counter)
		{
			std::filesystem::path path(file);
			std::ostringstream name;
			name << path.filename().replace_extension().string() << "_" << line << "_" << counter;
			return std::unique_ptr<RequestExecutorScope>(new RequestExecutorScope(name.str()));
		}
	};
}
