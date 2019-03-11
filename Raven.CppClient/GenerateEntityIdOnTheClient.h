#pragma once
#include <typeindex>
#include "DocumentConventions.h"

namespace ravendb::client::documents::identity
{
	class GenerateEntityIdOnTheClient
	{
	private:
		const std::shared_ptr<conventions::DocumentConventions> _conventions;
		const std::function<std::string(std::type_index, std::shared_ptr<void>)> _generate_id;

	public:
		GenerateEntityIdOnTheClient(std::shared_ptr<conventions::DocumentConventions> conventions,
			std::function<std::string(std::type_index, std::shared_ptr<void>)> generate_id);

		std::optional<std::string> try_get_id_from_instance(std::type_index type, std::shared_ptr<void> entity) const;

		std::string get_or_generate_document_id(std::type_index type, std::shared_ptr<void> entity) const;

		std::string generate_document_key_for_storage(std::type_index type, std::shared_ptr<void> entity) const;

		void try_set_identity(std::type_index type, std::shared_ptr<void> entity, const std::string& id) const;
	};
}
