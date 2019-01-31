#include "pch.h"
#include "re_definitions.h"
#include "CreateDatabaseOperation.h"
#include "DeleteDatabasesOperation.h"
#include <fstream>


namespace ravendb::client::tests::definitions
{
	//using fiddler + verbose
	void set_for_fiddler(CURL* curl)
	{
		curl_easy_setopt(curl, CURLOPT_PROXY, "127.0.0.1:8888");
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
	}

	void set_no_proxy(CURL* curl)
	{

		curl_easy_setopt(curl, CURLOPT_NOPROXY, "*");
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
	}

	ConnectionDetailsHolder::~ConnectionDetailsHolder() = default;

	ConnectionDetailsHolder::ConnectionDetailsHolder(const std::string& def_file_name, bool has_certificate = true)
	{
		//open definitions file
		std::ifstream def_file(def_file_name);
		if (!def_file)
		{
			throw std::runtime_error(std::string("Can't open ") + def_file_name);
		}
		if (!std::getline(def_file, url))
		{
			throw std::runtime_error(std::string("Can't read url from ") + def_file_name);
		}

		if(!has_certificate)
		{
			return;
		}
		//get certificate
		std::string cert_file_name;
		if(!std::getline(def_file, cert_file_name) || cert_file_name.empty())
		{
			throw std::runtime_error(std::string("Can't read certificate file name from ") + def_file_name);
		}
		std::ifstream cert_file(cert_file_name);
		if (!cert_file)
		{
			throw std::runtime_error(std::string("Can't open ") + cert_file_name);
		}
		{
			std::stringstream stream;
			stream << cert_file.rdbuf();
			cert_details.certificate = stream.str();
		}
		//get key
		std::string key_file_name;
		if (!std::getline(def_file, key_file_name) || key_file_name.empty())
		{
			throw std::runtime_error(std::string("Can't read key file name from ") + def_file_name);
		}
		std::ifstream key_file(key_file_name);
		if (!key_file)
		{
			throw std::runtime_error(std::string("Can't open ") + key_file_name);
		}
		{
			std::stringstream stream;
			stream << key_file.rdbuf();
			cert_details.key = stream.str();
		}
		//get CA file path(optional)
		std::string CA_path_file_name;
		if (!std::getline(def_file, CA_path_file_name))
		{
			return;//defaults will be used
		}
		//no problem if CA_path_file_name.empty() == true
		cert_details.ca_path = CA_path_file_name;

		//get password(optional)
		std::string key_password;
		if (!std::getline(def_file, key_password))
		{
			return;//no password
		}
		//no problem if  key_password.empty() == true
		cert_details.key_password = key_password;
	}

	RequestExecutorScope::RequestExecutorScope(std::string db_name, bool is_secured, bool use_fiddler)
		: _db_name(std::move(db_name))
	{
		auto server_wide_req_exec = get_raw_request_executor({}, is_secured, use_fiddler);

		ravendb::client::serverwide::DatabaseRecord rec{};
		rec.database_name = _db_name;
		serverwide::operations::CreateDatabaseOperation op(rec);
		server_wide_req_exec->execute(op.get_command({}));

		_executor = get_raw_request_executor(_db_name, is_secured, use_fiddler);
	}

	RequestExecutorScope::~RequestExecutorScope()
	{
		ravendb::client::serverwide::operations::DeleteDatabasesOperation op(_db_name, true, {}, std::chrono::seconds(20));
		_executor->execute(op.get_command({}));
	}


	//request executor only - no DB is created
	std::shared_ptr<ravendb::client::http::RequestExecutor> get_raw_request_executor
	(const std::string& db, bool is_secured, bool use_fiddler)
	{
		static const auto sec_conn_details = ConnectionDetailsHolder(SECURED_RE_DETAILS, true);
		static const auto unsec_conn_details = ConnectionDetailsHolder(UNSECURED_RE_DETAILS, false);
		
		return is_secured ?
			http::RequestExecutor::create({ sec_conn_details.get_url() }, db,
				sec_conn_details.get_cert_det(), set_no_proxy) :
			http::RequestExecutor::create({ unsec_conn_details.get_url() }, db,
				{}, use_fiddler ? set_for_fiddler : set_no_proxy);
	}

	std::shared_ptr<RequestExecutorScope> RequestExecutorScope::get_request_executor_with_db(
		const std::string& file, int line, int counter, bool is_secured, bool use_fiddler)
	{
		std::filesystem::path path(file);
		std::ostringstream name;
		name << path.filename().replace_extension().string() << "_" << line << "_" << counter;
		return is_secured ?
			std::make_shared<RequestExecutorScope>(name.str(), true, use_fiddler) :
			std::make_shared<RequestExecutorScope>(name.str(), false, use_fiddler);
	}

}

