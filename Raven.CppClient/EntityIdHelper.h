#pragma once
#include <typeindex>
#include <optional>

namespace ravendb::client
{
	class EntityIdHelper
	{
	public:
		using IdGet = std::function<std::optional<std::string>(std::type_index type, std::shared_ptr<void> entity)>;

		using IdSet = std::function<bool(std::type_index type, std::shared_ptr<void> entity, const std::string& id)>;

	private:
		IdGet _id_get;
		IdSet _id_set;
		std::string _id_field_name;

	public:
		EntityIdHelper(
			IdGet id_get,
			IdSet id_set,
			std::string id_field_name);

		std::optional<std::string> try_get_id(std::type_index type, std::shared_ptr<void> entity) const;

		bool try_set_id(std::type_index type, std::shared_ptr<void> entity, const std::string& id) const;

		const std::string& get_id_field_name() const;
	};
}
