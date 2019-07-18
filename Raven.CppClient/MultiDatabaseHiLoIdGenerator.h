#pragma once
#include <typeindex>
#include <shared_mutex>
#include "DocumentConventions.h"
#include "MultiTypeHiLoIdGenerator.h"

namespace ravendb::client::documents
{
	class DocumentStore;

	namespace conventions
	{
		class DocumentConventions;
	}
}

namespace ravendb::client::documents::identity
{

	class MultiDatabaseHiLoIdGenerator
	{
	private:
		std::shared_mutex _generators_lock{};

		std::unordered_map<std::string, std::unique_ptr<MultiTypeHiLoIdGenerator>> _generators{};

	protected:
		const std::weak_ptr<DocumentStore> store;
		const std::shared_ptr<conventions::DocumentConventions> conventions;

	public:
		MultiDatabaseHiLoIdGenerator(std::shared_ptr<DocumentStore> store_param, 
			std::shared_ptr<conventions::DocumentConventions> conventions_param);

		std::optional<std::string> generate_document_id(const std::optional<std::string>& db_name,
			std::type_index type, std::shared_ptr<void> entity);

		void return_unused_range(const DocumentStore& store) const;
	};
}
