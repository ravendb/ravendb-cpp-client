#pragma once
#include "CommandDataBase.h"
#include "json_utils.h"

namespace ravendb::client::documents::commands::batches
{
	class DeleteCommandData : public CommandDataBase
	{
	public:
		DeleteCommandData(std::string id, std::string change_vector)
			: CommandDataBase(std::move(id), {}, std::move(change_vector), CommandType::DELETE_)
		{}
		nlohmann::json serialize() const override
		{
			using ravendb::client::impl::utils::json_utils::set_val_to_json;
			nlohmann::json j{};

			set_val_to_json(j, "Id", get_id());
			if (!impl::utils::is_blank(get_change_vector()))
			{
				set_val_to_json(j, "ChangeVector", get_change_vector());
			}else
			{
				set_val_to_json(j, "ChangeVector", nullptr);
			}
			set_val_to_json(j, "Type", "DELETE");

			serialize_extra_fields(j);

			return j;
		}

		virtual void serialize_extra_fields(nlohmann::json& json) const
		{}//empty by design

		void on_before_save_changes(std::shared_ptr<session::InMemoryDocumentSessionOperations> session) override
		{}
	};
}
