#include "pch.h"
#include "ds_definitions.h"
#include "re_definitions.h"
#include "RequestExecutor.h"
#include "DatabaseRecord.h"
#include "CreateDatabaseOperation.h"
#include "DeleteDatabasesOperation.h"

namespace ravendb::client::tests::definitions
{
	DocumentStoreScope::DocumentStoreScope(const std::string& db_name, bool is_secured, bool use_fiddler)
	{
		static const auto sec_conn_details = ConnectionDetailsHolder(SECURED_RE_DETAILS, true);
		static const auto unsec_conn_details = ConnectionDetailsHolder(UNSECURED_RE_DETAILS, false);

		_document_store->set_database(db_name);

		if (is_secured)
		{
			_document_store->set_urls({ sec_conn_details.get_url() });
			_document_store->set_certificate_details(sec_conn_details.get_cert_det());
			_document_store->set_before_perform(set_no_proxy);
		}
		else
		{
			_document_store->set_urls({ unsec_conn_details.get_url() });
			_document_store->set_before_perform(use_fiddler ? set_for_fiddler : set_no_proxy);
		}

		_document_store->initialize();

		auto server_wide_req_exec = get_raw_request_executor({}, is_secured, use_fiddler);
		ravendb::client::serverwide::DatabaseRecord rec{};
		rec.database_name = _document_store->get_database();
		serverwide::operations::CreateDatabaseOperation op(rec);
		server_wide_req_exec->execute(op.get_command({}));
	}

	DocumentStoreScope::~DocumentStoreScope()
	{
		auto op = ravendb::client::serverwide::operations::DeleteDatabasesOperation(
			_document_store->get_database(), true, {}, std::chrono::seconds(20));
		_document_store->get_request_executor()->execute(op.get_command({}));
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
