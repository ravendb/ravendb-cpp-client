#pragma once
#include "re_definitions.h"
#include "DocumentStore.h"

namespace ravendb::client::tests
{
#ifndef GET_DOCUMENT_STORE
#define GET_DOCUMENT_STORE() DocumentStoreScope::get_document_store(__FILE__, __LINE__, __COUNTER__)
#endif

#ifndef GET_SECURED_DOCUMENT_STORE
#define GET_SECURED_DOCUMENT_STORE() DocumentStoreScope::get_document_store(__FILE__, __LINE__, __COUNTER__, true)
#endif

	class DocumentStoreScope
	{
	private:
		documents::DocumentStore _document_store{};
	public:
		explicit DocumentStoreScope(const std::string& db_name, bool is_secured = false)
		{
			static const auto sec_conn_details = ConnectionDetailsHolder(SECURED_RE_DETAILS, true);
			static const auto unsec_conn_details = ConnectionDetailsHolder(UNSECURED_RE_DETAILS, false);

			_document_store.set_database(db_name);

			if(is_secured)
			{
				_document_store.set_urls({ sec_conn_details.get_url() });
				_document_store.set_certificate_details(sec_conn_details.get_cert_det());
				_document_store.set_before_perform(set_verbose);
			}else
			{
				_document_store.set_urls({ unsec_conn_details.get_url() });
				_document_store.set_before_perform(set_for_fiddler);
			}

			_document_store.initialize();

			auto server_wide_req_exec = get_raw_request_executor(is_secured);
			ravendb::client::serverwide::DatabaseRecord rec{};
			rec.database_name = _document_store.get_database();
			serverwide::operations::CreateDatabaseOperation op(rec);
			server_wide_req_exec->execute(op.get_command({}));
		}

		~DocumentStoreScope()
		{
			auto op = ravendb::client::serverwide::operations::DeleteDatabasesOperation(
				_document_store.get_database(), true, {}, std::chrono::seconds(20));
			_document_store.get_request_executor()->execute(op.get_command({}));
		}

		documents::DocumentStore& get() noexcept
		{
			return _document_store;
		}

		static std::shared_ptr<DocumentStoreScope> get_document_store
		(const std::string& file, int line, int counter, bool is_secured = false)
		{
			std::filesystem::path path(file);
			std::ostringstream name;
			name << path.filename().replace_extension().string() << "_" << line << "_" << counter;
			return is_secured ?
				std::make_shared<DocumentStoreScope>(name.str(), true) :
				std::make_shared<DocumentStoreScope>(name.str());
		}
	};
}
