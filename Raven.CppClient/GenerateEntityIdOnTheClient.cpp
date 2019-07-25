#include "stdafx.h"
#include "GenerateEntityIdOnTheClient.h"

namespace ravendb::client::documents::identity
{
	GenerateEntityIdOnTheClient::GenerateEntityIdOnTheClient(
		std::shared_ptr<conventions::DocumentConventions> conventions,
		std::function<std::string(std::type_index, std::shared_ptr<void>)> generate_id)
		: _conventions(conventions)
		, _generate_id(std::move(generate_id))
	{}

	std::optional<std::string> GenerateEntityIdOnTheClient::try_get_id_from_instance(std::type_index type, std::shared_ptr<void> entity) const
	{
		if(!entity)
		{
			throw std::invalid_argument("entity cannot be empty");
		}

		auto id_helper = conventions::DocumentConventions::get_entity_id_helper(type);
		if(id_helper)
		{
			return id_helper->try_get_id(type, entity);
		}
		return {};
	}

	std::string GenerateEntityIdOnTheClient::get_or_generate_document_id(std::type_index type,
		std::shared_ptr<void> entity) const
	{
		auto&& id_from_instance = try_get_id_from_instance(type, entity);
		std::string id = !id_from_instance ? _generate_id(type, entity) : *id_from_instance;

		if(!id.empty() && id.front() == '/')
		{
			std::ostringstream msg{};
			msg << "Cannot use value '" << id << "' as a document id because it begins with a '/'";
			throw std::runtime_error(msg.str());
		}

		return id;
	}

	std::string GenerateEntityIdOnTheClient::generate_document_key_for_storage(std::type_index type,
		std::shared_ptr<void> entity) const
	{
		auto id = get_or_generate_document_id(type, entity);
		try_set_identity(type, entity, id);
		return id;
	}

	void GenerateEntityIdOnTheClient::try_set_identity(std::type_index type, std::shared_ptr<void> entity,
		const std::string& id) const
	{
		auto id_helper = conventions::DocumentConventions::get_entity_id_helper(type);
		if(id_helper)
		{
			id_helper->try_set_id(type, entity, id);
		}
	}
}
