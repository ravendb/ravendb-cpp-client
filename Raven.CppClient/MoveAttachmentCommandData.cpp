#include "stdafx.h"
#include "MoveAttachmentCommandData.h"
#include "json_utils.h"
#include "utils.h"

namespace ravendb::client::documents::commands::batches
{
	MoveAttachmentCommandData::MoveAttachmentCommandData(std::string document_id, std::string name,
		std::string destination_document_id, std::string destination_name, std::string change_vector)
		: CommandDataBase(std::move(document_id), std::move(name), std::move(change_vector),
			CommandType::ATTACHMENT_MOVE)
		, destination_id([&]
	{
		if (impl::utils::is_blank(destination_document_id))
		{
			throw std::invalid_argument("'destination_id' cannot be empty");
		}
		return std::move(destination_document_id);
	}())
		, destination_name([&]
	{
		if (impl::utils::is_blank(destination_name))
		{
			throw std::invalid_argument("'destination_name' cannot be empty");
		}
		return std::move(destination_name);
	}())
	{
		if (impl::utils::is_blank(get_id()))
		{
			throw std::invalid_argument("'document_id' cannot be empty");
		}
		if (impl::utils::is_blank(get_name()))
		{
			throw std::invalid_argument("'name' cannot be empty");
		}
	}

	nlohmann::json MoveAttachmentCommandData::serialize() const
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;
		nlohmann::json j = nlohmann::json::object();

		set_val_to_json(j, "Id", get_id());
		set_val_to_json(j, "Name", get_name());
		set_val_to_json(j, "DestinationId", get_destination_id());
		set_val_to_json(j, "DestinationName", get_destination_name());
		if (!impl::utils::is_blank(get_change_vector()))
		{
			set_val_to_json(j, "ChangeVector", get_change_vector());
		}
		else
		{
			set_val_to_json(j, "ChangeVector", nullptr);
		}
		set_val_to_json(j, "Type", get_type());

		return j;
	}
}
