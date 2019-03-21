#pragma once
#include "AbstractIndexCreationTask.h"

namespace ravendb::client::documents::indexes
{
	class IndexCreation
	{
	private:
		//TODO Log logger = LogFactory.getLog(IndexCreation.class);

	public:
		static void create_indexes(const std::vector<std::shared_ptr<AbstractIndexCreationTask>>& indexes,
			std::shared_ptr<IDocumentStore> store,
			std::shared_ptr<conventions::DocumentConventions> conventions = {});

		static std::vector<IndexDefinition> create_indexes_to_add(
			std::vector<std::shared_ptr<AbstractIndexCreationTaskBase>>& index_creation_tasks,
			std::shared_ptr<conventions::DocumentConventions> conventions);
	};
}
