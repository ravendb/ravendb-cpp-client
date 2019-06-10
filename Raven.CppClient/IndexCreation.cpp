#include "stdafx.h"
#include "IndexCreation.h"
#include "MaintenanceOperationExecutor.h"
#include "PutIndexesOperation.h"

namespace ravendb::client::documents::indexes
{
	std::vector<IndexDefinition> IndexCreation::create_indexes_to_add(
		std::vector<std::shared_ptr<AbstractIndexCreationTaskBase>>& index_creation_tasks,
		std::shared_ptr<conventions::DocumentConventions> conventions)
	{
		auto definitions = std::vector<IndexDefinition>();
		for(auto& task : index_creation_tasks)
		{
			task->set_conventions(conventions);
			auto&& definition = task->create_index_definition();
			definition.name = task->get_index_name();
			definition.priority = task->get_priority() ? *task->get_priority() : IndexPriority::NORMAL;
			definitions.push_back(std::move(definition));
		}

		return std::move(definitions);
	}

	void IndexCreation::create_indexes(const std::vector<std::shared_ptr<AbstractIndexCreationTask>>& indexes,
		std::shared_ptr<IDocumentStore> store, std::shared_ptr<conventions::DocumentConventions> conventions)
	{
		if(!conventions)
		{
			conventions = store->get_conventions();
		}

		try
		{
			std::vector<std::shared_ptr<AbstractIndexCreationTaskBase>> tasks;
			std::transform(indexes.cbegin(), indexes.cend(), std::back_inserter(tasks),
				[](const std::shared_ptr<AbstractIndexCreationTask>& task)
			{
				return std::static_pointer_cast<AbstractIndexCreationTaskBase>(task);
			});
			auto&& indexes_to_add = create_indexes_to_add(tasks, conventions);
			store->maintenance()->send(operations::indexes::PutIndexesOperation(std::move(indexes_to_add)));
		}
		catch (std::exception& )
		{
			//TODO logger.info("Could not create indexes in one shot (maybe using older version of RavenDB ?)", e);

			for(auto& task : indexes)
			{
				task->execute(store, conventions);
			}
		}
	}
}
