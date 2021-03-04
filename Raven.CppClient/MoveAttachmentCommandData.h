#pragma once
#include "CommandDataBase.h"
#include "nlohmann/json.hpp"

namespace ravendb::client::documents::commands::batches
{
	class MoveAttachmentCommandData : public CommandDataBase
	{
	private:
		const std::string destination_id;
		const std::string destination_name;

	public:
		MoveAttachmentCommandData(std::string document_id, std::string name,
			std::string destination_document_id, std::string destination_name,
			std::string change_vector = "");

		const std::string& get_destination_id() const
		{
			return destination_id;
		}
		const std::string& get_destination_name() const
		{
			return destination_name;
		}

		nlohmann::json serialize() const override;

		void on_before_save_changes(std::shared_ptr<session::InMemoryDocumentSessionOperations> session) override
		{}
	};
}
