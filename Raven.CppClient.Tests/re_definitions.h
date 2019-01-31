#pragma once
#include <curl/curl.h>
#include "CertificateDetails.h"
#include "RequestExecutor.h"

namespace ravendb::client::tests::definitions
{
#ifndef GET_REQUEST_EXECUTOR
	#ifdef __USE_FIDDLER__
		#define GET_REQUEST_EXECUTOR() RequestExecutorScope::get_request_executor_with_db(__FILE__, __LINE__, __COUNTER__, false, true)
	#else
		#define GET_REQUEST_EXECUTOR() RequestExecutorScope::get_request_executor_with_db(__FILE__, __LINE__, __COUNTER__)
	#endif
#endif

#ifndef GET_SECURED_REQUEST_EXECUTOR
#define GET_SECURED_REQUEST_EXECUTOR() RequestExecutorScope::get_request_executor_with_db(__FILE__, __LINE__, __COUNTER__, true)
#endif

	constexpr char UNSECURED_RE_DETAILS[] = "../unsecured_re.txt";
	constexpr char SECURED_RE_DETAILS[] = "../secured_re.txt";

	//using fiddler + verbose
	void set_for_fiddler(CURL* curl);

	void set_no_proxy(CURL* curl);

	//request _executor only - no DB is created
	std::shared_ptr<ravendb::client::http::RequestExecutor> get_raw_request_executor(
		const std::string& db = {}, bool is_secured = false, bool use_fiddler = false);

	class ConnectionDetailsHolder
	{
	private:
		std::string url{};
		ravendb::client::impl::CertificateDetails cert_details{};

	public:
		~ConnectionDetailsHolder();
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
		std::shared_ptr<ravendb::client::http::RequestExecutor> _executor;
	public:
		explicit RequestExecutorScope(std::string db_name, bool is_secured = false, bool use_fiddler = false);

		~RequestExecutorScope();

		ravendb::client::http::RequestExecutor& get() const noexcept
		{
			return *_executor.get();
		}

		std::shared_ptr<ravendb::client::http::RequestExecutor> get_shared() const noexcept
		{
			return _executor;
		}

		const std::string& get_db_name() const
		{
			return _db_name;
		}

		static std::shared_ptr<RequestExecutorScope> get_request_executor_with_db(
			const std::string& file, int line, int counter, bool is_secured = false, bool use_fiddler = false);
	};
}
