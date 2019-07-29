#pragma once
#include "EntityIdHelper.h"
#include "utils.h"
#include "DocumentConventions.h"
#include <optional>

//The Id field named 'ID_FIELD_NAME' should be of the type std::string or std::optional<std::string>
#define REGISTER_ID_PROPERTY_FOR(TYPE, ID_FIELD_NAME)\
do\
{\
	ravendb::client::documents::conventions::DocumentConventions::add_entity_id_helper(typeid(TYPE), ravendb::client::EntityIdHelper(\
		[](std::type_index type, std::shared_ptr<void> entity)->std::optional<std::string>\
	{\
		auto typed_entity = std::static_pointer_cast<TYPE>(entity);\
		if (!entity)\
		{\
			throw std::invalid_argument("'entity' is empty or of incompatible type");\
		}\
		if constexpr(std::is_same_v<decltype(typed_entity->ID_FIELD_NAME), std::optional<std::string>>)\
			/*useless cast to shut the compiler*/ \
			if(static_cast<std::optional<std::string>>(typed_entity->ID_FIELD_NAME).has_value())\
			{\
				return typed_entity->ID_FIELD_NAME;\
			}\
		if constexpr(std::is_same_v<decltype(typed_entity->ID_FIELD_NAME), std::string>)\
			if (!ravendb::client::impl::utils::is_blank(typed_entity->ID_FIELD_NAME))\
			{\
				return typed_entity->ID_FIELD_NAME;\
			}\
		return {};\
	},\
		[](std::type_index type, std::shared_ptr<void> entity, const std::string& id)->bool\
	{\
		auto typed_entity = std::static_pointer_cast<TYPE>(entity);\
		if (!entity)\
		{\
			throw std::invalid_argument("'entity' is empty or of incompatible type");\
		}\
		if constexpr(std::is_same_v<decltype(typed_entity->ID_FIELD_NAME), std::optional<std::string>> ||\
			std::is_same_v<decltype(typed_entity->ID_FIELD_NAME), std::string>)\
		{\
			typed_entity->ID_FIELD_NAME = id;\
			return true;\
		}\
		return false;\
	}, #ID_FIELD_NAME));\
}while(false)
