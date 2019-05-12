#include "pch.h"
#include "ds_definitions.h"
#include "re_definitions.h"
#include "raven_test_definitions.h"
#include "RequestExecutor.h"
#include "DatabaseRecord.h"
#include "CreateDatabaseOperation.h"
#include "DeleteDatabasesOperation.h"
#include "ConnectionDetailsHolder.h"
#include "raven_test_definitions.h"

namespace ravendb::client::tests::definitions
{	
	DocumentStoreScope::DocumentStoreScope(const std::string& db_name, bool is_secured, bool use_fiddler)
	{
		static const auto sec_conn_details = infrastructure::ConnectionDetailsHolder(
			infrastructure::SECURED_RE_DETAILS_FILE_NAME, true);
		static const auto unsec_conn_details = infrastructure::ConnectionDetailsHolder(
			infrastructure::UNSECURED_RE_DETAILS_FILE_NAME, false);

		_document_store->set_database(db_name);

		if (is_secured)
		{
			_document_store->set_urls({ sec_conn_details.get_url() });
			_document_store->set_certificate_details(sec_conn_details.get_certificate_details());
			_document_store->set_before_perform(infrastructure::set_no_proxy);
		}
		else
		{
			_document_store->set_urls({ unsec_conn_details.get_url() });
			_document_store->set_before_perform(use_fiddler ? infrastructure::set_for_fiddler : infrastructure::set_no_proxy);
		}

		_document_store->get_conventions()->set_find_collection_name([](std::type_index type)->std::optional<std::string>
		{
			if (std::string_view(type.name()).find("Hilo") != std::string_view::npos)
				return "@hilo";
			else
				return {};
		});

		// todo: add ability to call a "customize document store" function for each test

		_document_store->initialize();

		auto server_wide_req_exec = get_raw_request_executor({}, is_secured, use_fiddler);
		ravendb::client::serverwide::DatabaseRecord rec{};
		rec.database_name = _document_store->get_database();
		serverwide::operations::CreateDatabaseOperation op(rec);
		server_wide_req_exec->execute(*op.get_command({}));
	}

	DocumentStoreScope::~DocumentStoreScope()
	{
		try
		{
			auto op = ravendb::client::serverwide::operations::DeleteDatabasesOperation(
				_document_store->get_database(), true, {}, std::chrono::seconds(20));
			_document_store->get_request_executor()->execute(*op.get_command({}));
		}
		catch (...)
		{}
	}

	std::shared_ptr<DocumentStoreScope> DocumentStoreScope::get_document_store(
		const std::string& file, int line, int counter, bool is_secured, bool use_fiddler)
	{
		std::filesystem::path path(file);
		std::ostringstream name;
		name << path.filename().replace_extension().string() << "_" << line << "_" << counter;
		return is_secured ?
			std::make_shared<DocumentStoreScope>(name.str(), true, use_fiddler) :
			std::make_shared<DocumentStoreScope>(name.str(), false, use_fiddler);
	}
}
