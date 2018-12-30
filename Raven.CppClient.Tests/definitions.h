#pragma once
#include <curl/curl.h>
#include <filesystem>

#include "RequestExecutor.h"
#include "CreateDatabaseOperation.h"
#include "DeleteDatabasesOperation.h"

namespace ravendb::client::tests
{
#ifndef GET_REQUEST_EXECUTOR
#define GET_REQUEST_EXECUTOR() RequestExecutorScope::get_request_executor_with_db(__FILE__, __LINE__, __COUNTER__)
#endif

#ifndef GET_SECURED_REQUEST_EXECUTOR
#define GET_SECURED_REQUEST_EXECUTOR() RequestExecutorScope::get_request_executor_with_db(__FILE__, __LINE__, __COUNTER__, true)
#endif

	constexpr char UNSECURED_RE_DETAILS[] = "../unsecured_re.txt";
	constexpr char SECURED_RE_DETAILS[] = "../secured_re.txt";

	//using fiddler + verbose
	void set_for_fiddler(CURL* curl);

	void set_verbose(CURL* curl);

	//request _executor only - no DB is created
	std::unique_ptr<ravendb::client::http::RequestExecutor> get_raw_request_executor
	(bool is_secured = false, const std::string& db = {});

	class ConnectionDetailsHolder
	{
	private:
		std::string url{};
		ravendb::client::impl::CertificateDetails cert_details{};

	public:
		~ConnectionDetailsHolder() = default;
		ConnectionDetailsHolder(const std::string& def_file_name, bool has_certificate);

		const ravendb::client::impl::CertificateDetails& get_cert_det() const
		{
			return cert_details;
		}
		const std::string& get_url() const
		{
			return url;
		}
	};

	class RequestExecutorScope
	{
	private:
		std::string _db_name;
		std::unique_ptr<ravendb::client::http::RequestExecutor> _executor;
	public:
		RequestExecutorScope(std::string db_name, bool is_secured = false)
			: _db_name(std::move(db_name))
		{
			auto server_wide_req_exec = get_raw_request_executor(is_secured);

			ravendb::client::serverwide::DatabaseRecord rec{};
			rec.database_name = _db_name;
			serverwide::operations::CreateDatabaseOperation op(rec);
			server_wide_req_exec->execute(op.get_command({}));

			_executor = get_raw_request_executor(is_secured, _db_name);
		}

		~RequestExecutorScope()
		{
			ravendb::client::serverwide::operations::DeleteDatabasesOperation op(_db_name, true, {}, std::chrono::seconds(20));
			_executor->execute(op.get_command({}));
		}

		ravendb::client::http::RequestExecutor* get() const noexcept
		{
			return _executor.get();
		}

		const std::string& get_db_name() const
		{
			return _db_name;
		}

		static std::unique_ptr<RequestExecutorScope> get_request_executor_with_db
			(const std::string& file, int line, int counter, bool is_secured = false)
		{
			std::filesystem::path path(file);
			std::ostringstream name;
			name << path.filename().replace_extension().string() << "_" << line << "_" << counter;
			return is_secured ?
				std::make_unique<RequestExecutorScope>(name.str(), true) :
				std::make_unique<RequestExecutorScope>(name.str());
		}
	};
}
