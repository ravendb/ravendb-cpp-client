#include "stdafx.h"
#include "MultiDatabaseHiLoIdGenerator.h"
#include "DocumentStore.h"
#include "MultiTypeHiLoIdGenerator.h"

namespace ravendb::client::documents::identity
{
	MultiDatabaseHiLoIdGenerator::MultiDatabaseHiLoIdGenerator(std::shared_ptr<DocumentStore> store_param,
		std::shared_ptr<conventions::DocumentConventions> conventions_param)
		: _generators()
		, store(store_param)
		, conventions(conventions_param)
	{}

	std::optional<std::string> MultiDatabaseHiLoIdGenerator::generate_document_id(
		const std::optional<std::string>& db_name, std::type_index type, std::shared_ptr<void> entity)
	{
		auto&& db = db_name ? *db_name : store->get_database();

		{
			std::shared_lock<std::shared_mutex> lock(_generators_lock);

			if (auto it = _generators.find(db);
				it != _generators.end())
			{
				return it->second->generate_document_id(type, entity);
			}
		}
		{
			std::unique_lock<std::shared_mutex> lock(_generators_lock);

			if (auto it = _generators.find(db);
				it != _generators.end())
			{
				return it->second->generate_document_id(type, entity);
			}
			else
			{
				_generators.insert_or_assign(db, std::make_unique<MultiTypeHiLoIdGenerator>(store, db, conventions));
			}
		}
		return _generators.at(db)->generate_document_id(type, entity);
	}

	void MultiDatabaseHiLoIdGenerator::return_unused_range() const
	{
		for(const auto& [db, generator] : _generators)
		{
			generator->return_unused_range();
		}
	}
}
