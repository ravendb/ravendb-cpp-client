#pragma once
#include "DocumentStore.h"

namespace ravendb::client::tests::utils
{
	void delete_all_documents(std::shared_ptr<documents::DocumentStore> store);

	void delete_all_indexes(std::shared_ptr<documents::DocumentStore> store);
}
