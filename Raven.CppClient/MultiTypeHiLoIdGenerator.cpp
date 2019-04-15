#include "stdafx.h"
#include "MultiTypeHiLoIdGenerator.h"
#include "DocumentStore.h"
#include "HiLoIdGenerator.h"
#include "DocumentConventions.h"
#include "utils.h"

namespace ravendb::client::documents::identity
{
	std::unique_ptr<HiLoIdGenerator> MultiTypeHiLoIdGenerator::create_generator_for(std::string tag) const
	{
		return std::make_unique<HiLoIdGenerator>(tag, store.lock(), db_name, conventions->get_identity_part_separator());
	}

	MultiTypeHiLoIdGenerator::MultiTypeHiLoIdGenerator(std::shared_ptr<DocumentStore> store_param,
		std::string db_name_param, std::shared_ptr<conventions::DocumentConventions> conventions_param)
		: _id_generator_by_tag()
		, store(store_param)
		, db_name(std::move(db_name_param))
		, conventions(conventions_param)
	{}

	std::optional<std::string> MultiTypeHiLoIdGenerator::generate_document_id(std::type_index type,
		std::shared_ptr<void> entity)
	{
		auto&& type_tag_name = conventions->get_collection_name(type);
		if(impl::utils::is_blank(type_tag_name))
		{
			return {};
		}

		auto&& tag = conventions->get_transform_class_collection_name_to_document_id_prefix()(type_tag_name);

		{
			std::shared_lock<std::shared_mutex> lock(_generator_lock);

			if(auto it = _id_generator_by_tag.find(tag);
				it != _id_generator_by_tag.end())
			{
				return it->second->generate_document_id();
			}
		}
		{
			std::unique_lock<std::shared_mutex> lock(_generator_lock);

			if (auto it = _id_generator_by_tag.find(tag);
				it != _id_generator_by_tag.end())
			{
				return it->second->generate_document_id();
			}
			else
			{
				_id_generator_by_tag.insert_or_assign(tag, create_generator_for(tag));
			}
			return _id_generator_by_tag.at(tag)->generate_document_id();
		}
	}

	void MultiTypeHiLoIdGenerator::return_unused_range(const DocumentStore& store) const
	{
		for(const auto& [tag, generator] : _id_generator_by_tag)
		{
			generator->return_unused_range(store);
		}
	}
}
