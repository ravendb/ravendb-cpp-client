#pragma once
#include "CommandDataBase.h"
#include "json.hpp"
#include "json_utils.h"
#include "utils.h"


namespace ravendb::client::documents::commands::batches
{
	class CopyAttachmentCommandData : public CommandDataBase
	{
	private:
		const std::string destination_id;
		const std::string destination_name;

	public:
		CopyAttachmentCommandData(std::string source_document_id, std::string source_name,
			std::string destination_document_id, std::string destination_name,
			std::string change_vector = "")
			: CommandDataBase(std::move(source_document_id), std::move(source_name), std::move(change_vector),
				CommandType::ATTACHMENT_COPY)
			, destination_id([&]
		{
			if (impl::utils::is_blank(destination_id))
			{
				throw std::invalid_argument("'destination_id' cannot be empty");
			}
			return std::move(destination_id);
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
				throw std::invalid_argument("'source_document_id' cannot be empty");
			}
			if (impl::utils::is_blank(get_name()))
			{
				throw std::invalid_argument("'source_name' cannot be empty");
			}
		}

		const std::string& get_destination_id() const
		{
			return destination_id;
		}
		const std::string& get_destination_name() const
		{
			return destination_name;
		}

		nlohmann::json serialize() const override
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

		void on_before_save_changes(std::shared_ptr<session::InMemoryDocumentSessionOperations> session) override
		{}
	};
}
