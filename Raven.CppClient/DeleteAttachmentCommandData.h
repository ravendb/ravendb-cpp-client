#pragma once
#include "CommandDataBase.h"
#include "nlohmann/json.hpp"

namespace ravendb::client::documents::commands::batches
{
	class DeleteAttachmentCommandData : public CommandDataBase
	{
	public:
		DeleteAttachmentCommandData(std::string document_id, std::string name, std::string change_vector = "");

		nlohmann::json serialize() const override;

		void on_before_save_changes(std::shared_ptr<session::InMemoryDocumentSessionOperations> session) override
		{}
	};
}
