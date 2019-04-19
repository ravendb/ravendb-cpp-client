#pragma once
#include <typeindex>
#include <optional>
#include "utils.h"

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

	inline EntityIdHelper::EntityIdHelper(IdGet id_get_param, IdSet id_set_param, std::string id_field_name)
		: _id_get(id_get_param)
		, _id_set(id_set_param)
		, _id_field_name(std::move(id_field_name))
	{
		if(impl::utils::is_blank(_id_field_name))
		{
			throw std::invalid_argument("id_field_name can't be blank");
		}
	}

	inline std::optional<std::string> EntityIdHelper::try_get_id(std::type_index type, std::shared_ptr<void> entity) const
	{
		return _id_get(type, entity);
	}

	inline bool EntityIdHelper::try_set_id(std::type_index type, std::shared_ptr<void> entity, const std::string& id) const
	{
		return _id_set(type, entity, id);
	}

	inline const std::string& EntityIdHelper::get_id_field_name() const
	{
		return _id_field_name;
	}
}
