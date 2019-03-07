#pragma once

namespace ravendb::client
{
	class EntityIdHelper
	{
	public:
		using IdGet = std::function<std::string(std::shared_ptr<void> entity)>;

		using IdSet = std::function<void(std::shared_ptr<void> entity, const std::string& id)>;

	private:
		IdGet id_get;
		IdSet id_set;

	public:
		EntityIdHelper(
			IdGet id_get_param,
			IdSet id_set_param);

		std::string get_id(std::shared_ptr<void> entity) const;

		void set_id(std::shared_ptr<void> entity, const std::string& id) const;
	};

	inline EntityIdHelper::EntityIdHelper(IdGet id_get_param, IdSet id_set_param)
		: id_get(id_get_param)
		, id_set(id_set_param)
	{}

	inline std::string EntityIdHelper::get_id(std::shared_ptr<void> entity) const
	{
		return id_get(entity);
	}

	inline void EntityIdHelper::set_id(std::shared_ptr<void> entity, const std::string& id) const
	{
		id_set(entity, id);
	}
}
