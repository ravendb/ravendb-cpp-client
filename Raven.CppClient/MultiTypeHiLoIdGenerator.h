#pragma once
#include <shared_mutex>
#include <typeindex>
#include "HiLoIdGenerator.h"

namespace ravendb::client::documents
{
	class DocumentStore;

	namespace identity
	{
		class HiLoIdGenerator;
	}

	namespace conventions
	{
		class DocumentConventions;
	}
}

namespace ravendb::client::documents::identity
{
	class MultiTypeHiLoIdGenerator
	{
	private:
		std::shared_mutex _generator_lock{};
		std::unordered_map<std::string, std::unique_ptr<HiLoIdGenerator>> _id_generator_by_tag;

	protected:
		const std::weak_ptr<DocumentStore> store;
		const std::string db_name;
		const std::shared_ptr<conventions::DocumentConventions> conventions;

		virtual std::unique_ptr<HiLoIdGenerator> create_generator_for(std::string tag) const;

	public:
		virtual ~MultiTypeHiLoIdGenerator() = default;

		MultiTypeHiLoIdGenerator(std::shared_ptr<DocumentStore> store_param, std::string db_name_param,
			std::shared_ptr<conventions::DocumentConventions> conventions_param);

		std::optional<std::string> generate_document_id(std::type_index type, std::shared_ptr<void> entity);

		void return_unused_range(const DocumentStore& store) const;
	};
}
