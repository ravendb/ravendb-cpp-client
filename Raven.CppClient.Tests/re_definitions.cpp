#include "pch.h"
#include "re_definitions.h"
#include "CreateDatabaseOperation.h"
#include "DeleteDatabasesOperation.h"
#include "ConnectionDetailsHolder.h"
#include "raven_test_definitions.h"
#include "TasksExecutor.h"
#include "TasksScheduler.h"
#include "RequestExecutor.h"


#ifndef __has_include
  static_assert(false, "__has_include not supported");
#else
#  if __has_include(<filesystem>)
#    include <filesystem>
     namespace fs = std::filesystem;
#  elif __has_include(<experimental/filesystem>)
#    include <experimental/filesystem>
     namespace fs = std::experimental::filesystem;
#  elif __has_include(<boost/filesystem.hpp>)
#    include <boost/filesystem.hpp>
     namespace fs = boost::filesystem;
#  endif
#endif

namespace ravendb::client::tests::definitions
{
	RequestExecutorScope::RequestExecutorScope(std::string db_name, bool is_secured, bool use_fiddler)
		: _db_name(std::move(db_name))
	{
		auto server_wide_req_exec = get_raw_request_executor({}, is_secured, use_fiddler);

		ravendb::client::serverwide::DatabaseRecord rec{};
		rec.database_name = _db_name;
		serverwide::operations::CreateDatabaseOperation op(rec);
		
		auto document_conventions = server_wide_req_exec->get_conventions();
		auto command = op.get_command(document_conventions);

		server_wide_req_exec->execute(*command);

		_executor = get_raw_request_executor(_db_name, is_secured, use_fiddler);
	}

	RequestExecutorScope::~RequestExecutorScope()
	{
		try
		{
			ravendb::client::serverwide::operations::DeleteDatabasesOperation op(_db_name, true, {}, std::chrono::seconds(20));
			_executor->execute(*op.get_command(_executor->get_conventions()));
		}
		catch (...)
		{}
	}


	//request executor only - no DB is created
	std::shared_ptr<ravendb::client::http::RequestExecutor> get_raw_request_executor
	(const std::string& db, bool is_secured, bool use_fiddler)
	{
		static const auto sec_conn_details = infrastructure::ConnectionDetailsHolder(infrastructure::SECURED_RE_DETAILS_FILE_NAME, true);
		static const auto unsec_conn_details = infrastructure::ConnectionDetailsHolder(infrastructure::UNSECURED_RE_DETAILS_FILE_NAME, false);

		if (db.empty())
		{
			auto conventions = documents::conventions::DocumentConventions::create();
			conventions->set_disable_topology_updates(true);
			conventions->freeze();

			return is_secured ?
				http::RequestExecutor::create_for_single_node_with_configuration_updates(
					{ sec_conn_details.get_url() }, db, sec_conn_details.get_certificate_details(),
					std::make_shared<impl::TasksScheduler>(std::make_shared<impl::TasksExecutor>()),
					conventions,
					infrastructure::set_no_proxy) :
				http::RequestExecutor::create_for_single_node_with_configuration_updates(
					{ unsec_conn_details.get_url() }, db, {},
					std::make_shared<impl::TasksScheduler>(std::make_shared<impl::TasksExecutor>()),
					conventions,
					use_fiddler ? infrastructure::set_for_fiddler : infrastructure::set_no_proxy, {});
		}
		return is_secured ?
			http::RequestExecutor::create(
				{ sec_conn_details.get_url() }, db, sec_conn_details.get_certificate_details(),
				std::make_shared<impl::TasksScheduler>(std::make_shared<impl::TasksExecutor>()),
				documents::conventions::DocumentConventions::default_conventions(),
				infrastructure::set_no_proxy) :
			http::RequestExecutor::create(
				{ unsec_conn_details.get_url() }, db, {},
				std::make_shared<impl::TasksScheduler>(std::make_shared<impl::TasksExecutor>()),
				documents::conventions::DocumentConventions::default_conventions(),
				use_fiddler ? infrastructure::set_for_fiddler : infrastructure::set_no_proxy, {});

	}

	std::shared_ptr<RequestExecutorScope> RequestExecutorScope::get_request_executor_with_db(
		const std::string& file, int line, int counter, bool is_secured, bool use_fiddler)
	{
		fs::path path(file);
		std::ostringstream name;
		name << path.filename().replace_extension().string() << "_" << line << "_" << counter;
		try{
			return is_secured ?
				std::make_shared<RequestExecutorScope>(name.str(), true, use_fiddler) :
				std::make_shared<RequestExecutorScope>(name.str(), false, use_fiddler);
		}
		catch(std::exception& e)
		{
			std::throw_with_nested(std::runtime_error(std::string("Failed to get request executor. Reason:") + e.what()));
		}
	}

}

