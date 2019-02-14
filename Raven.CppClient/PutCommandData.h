#pragma once
#include "CommandDataBase.h"
#include "json_utils.h"

namespace ravendb::client::documents::commands::batches
{
	class PutCommandData : public CommandDataBase
	{
	private:
		const nlohmann::json _document;

	public:
		PutCommandData(std::string id, std::string change_vector, nlohmann::json document)
			: CommandDataBase(std::move(id), {}, std::move(change_vector), CommandType::PUT)
			, _document(std::move(document))
		{}

		nlohmann::json serialize() const override
		{
			using ravendb::client::impl::utils::json_utils::set_val_to_json;
			nlohmann::json j = nlohmann::json::object();

			set_val_to_json(j, "Id", get_id());
			if (!impl::utils::is_blank(get_change_vector()))
			{
				set_val_to_json(j, "ChangeVector", get_change_vector());
			}
			else
			{
				set_val_to_json(j, "ChangeVector", nullptr);
			}
			set_val_to_json(j, "Document", _document);
			set_val_to_json(j, "Type", "PUT");
			
			return j;
		}

		void on_before_save_changes(session::InMemoryDocumentSessionOperations& session) override
		{}
	};
}
