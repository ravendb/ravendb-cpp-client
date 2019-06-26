#pragma once
#include "CommandDataBase.h"
#include "json_utils.h"
#include "utils.h"
#include "Raven.CppClient.Tests/Order.h"

namespace ravendb::client::documents::commands::batches
{
	class PutAttachmentCommandData : public CommandDataBase
	{
	private:
		const std::shared_ptr<const std::vector<std::byte>> _data;
		const std::optional<std::string> _content_type;

	public:
		PutAttachmentCommandData(std::string document_id, std::string name, std::shared_ptr<const std::vector<std::byte>> data, 
			std::optional<std::string> content_type, std::string change_vector)
			: CommandDataBase(std::move(document_id), std::move(name), std::move(change_vector),
				CommandType::ATTACHMENT_PUT)
			, _data(data)
			, _content_type(std::move(content_type))
		{
			if(impl::utils::is_blank(get_id()))
			{
				throw std::invalid_argument("'document_id' cannot be empty");
			}
			if (impl::utils::is_blank(get_name()))
			{
				throw std::invalid_argument("'name' cannot be empty");
			}
		}

		nlohmann::json serialize() const override
		{
			using ravendb::client::impl::utils::json_utils::set_val_to_json;
			nlohmann::json j = nlohmann::json::object();

			set_val_to_json(j, "Id", get_id());
			set_val_to_json(j, "Name", get_name());
			set_val_to_json(j, "ContentType", _content_type);
			if (!impl::utils::is_blank(get_change_vector()))
			{
				set_val_to_json(j, "ChangeVector", get_change_vector());
			}
			else
			{
				set_val_to_json(j, "ChangeVector", nullptr);
			}
			set_val_to_json(j, "Type", "AttachmentPUT");

			return j;
		}

		void on_before_save_changes(std::shared_ptr<session::InMemoryDocumentSessionOperations> session) override
		{}
	};
}
