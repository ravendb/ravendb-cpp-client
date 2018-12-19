#include "pch.h"
#include "definitions.h"
#include <fstream>

namespace ravendb::client::tests
{
	//using fiddler + verbose
	void set_for_fiddler(CURL* curl)
	{
		curl_easy_setopt(curl, CURLOPT_PROXY, "127.0.0.1:8888");
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
	}

	void set_verbose(CURL* curl)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
	}

	std::shared_ptr<ravendb::client::impl::CertificateDetails> get_cert()
	{
		static std::shared_ptr<ravendb::client::impl::CertificateDetails> cert_det{};

		if (!cert_det)
		{
			cert_det = std::make_shared<ravendb::client::impl::CertificateDetails>();
			std::ifstream cert_file(R"(c:\work\PowerUser\PowerUser.crt)");
			std::ifstream key_file(R"(c:\work\PowerUser\PowerUser.key)");
			{
				std::stringstream stream;
				stream << cert_file.rdbuf();
				cert_det->certificate = stream.str();
			}
			{
				std::stringstream stream;
				key_file >> stream.rdbuf();
				cert_det->key = stream.str();
			}
			cert_det->key_password.emplace("PowerUser");
		}
		return cert_det;
	}

	//request executor only - no DB is created
	std::unique_ptr<ravendb::client::http::RequestExecutor> get_raw_request_executor
	(bool is_secured, const std::string& db)
	{
		return is_secured ? http::RequestExecutor::create({ SECURED_RAVEN_SERVER_URL }, db, *get_cert(), set_verbose) :
			http::RequestExecutor::create({ RAVEN_SERVER_URL }, db, {}, set_for_fiddler);
	}
}

