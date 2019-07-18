#include "stdafx.h"
#include "EntityIdHelper.h"
#include "utils.h"

namespace ravendb::client
{
	EntityIdHelper::EntityIdHelper(IdGet id_get_param, IdSet id_set_param, std::string id_field_name)
		: _id_get(id_get_param)
		, _id_set(id_set_param)
		, _id_field_name(std::move(id_field_name))
	{
		if (impl::utils::is_blank(_id_field_name))
		{
			throw std::invalid_argument("id_field_name can't be blank");
		}
	}

	std::optional<std::string> EntityIdHelper::try_get_id(std::type_index type, std::shared_ptr<void> entity) const
	{
		return _id_get(type, entity);
	}

	bool EntityIdHelper::try_set_id(std::type_index type, std::shared_ptr<void> entity, const std::string& id) const
	{
		return _id_set(type, entity, id);
	}

	const std::string& EntityIdHelper::get_id_field_name() const
	{
		return _id_field_name;
	}
}
