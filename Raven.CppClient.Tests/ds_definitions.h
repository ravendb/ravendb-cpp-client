#pragma once
#include "DocumentStore.h"

namespace ravendb::client::tests::definitions
{
#ifndef GET_DOCUMENT_STORE
	#ifdef __USE_FIDDLER__
		#define GET_DOCUMENT_STORE() DocumentStoreScope::get_document_store(__FILE__, __LINE__, __COUNTER__, false, true)
	#else 
		#define GET_DOCUMENT_STORE() DocumentStoreScope::get_document_store(__FILE__, __LINE__, __COUNTER__)
	#endif
#endif

#ifndef GET_SECURED_DOCUMENT_STORE
#define GET_SECURED_DOCUMENT_STORE() DocumentStoreScope::get_document_store(__FILE__, __LINE__, __COUNTER__, true, false)
#endif

	class DocumentStoreScope
	{
	private:
		std::shared_ptr<documents::DocumentStore> _document_store = documents::DocumentStore::create();
	public:
		explicit DocumentStoreScope(const std::string& db_name, bool is_secured = false, bool use_fiddler = false);

		~DocumentStoreScope();

		std::shared_ptr<documents::DocumentStore> get() noexcept
		{
			return _document_store;
		}

		static std::shared_ptr<DocumentStoreScope> get_document_store(
			const std::string& file, int line, int counter, bool is_secured = false, bool use_fiddler = false);
	};
}
