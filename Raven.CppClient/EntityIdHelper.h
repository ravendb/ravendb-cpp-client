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
		IdGet id_get;
		IdSet id_set;

	public:
		EntityIdHelper(
			IdGet id_get_param,
			IdSet id_set_param);

		std::optional<std::string> try_get_id(std::type_index type, std::shared_ptr<void> entity) const;

		bool try_set_id(std::type_index type, std::shared_ptr<void> entity, const std::string& id) const;
	};

	inline EntityIdHelper::EntityIdHelper(IdGet id_get_param, IdSet id_set_param)
		: id_get(id_get_param)
		, id_set(id_set_param)
	{}

	inline std::optional<std::string> EntityIdHelper::try_get_id(std::type_index type, std::shared_ptr<void> entity) const
	{
		return id_get(type, entity);
	}

	inline bool EntityIdHelper::try_set_id(std::type_index type, std::shared_ptr<void> entity, const std::string& id) const
	{
		return id_set(type, entity, id);
	}
}
