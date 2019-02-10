#pragma once

namespace ravendb::client::tests::definitions
{
	class DocumentStoreScope;
}

namespace ravendb::client::tests::utils
{
	void delete_all_documents(std::shared_ptr<definitions::DocumentStoreScope> test_suite_store);

	void delete_all_indexes(std::shared_ptr<definitions::DocumentStoreScope> test_suite_store);
}
