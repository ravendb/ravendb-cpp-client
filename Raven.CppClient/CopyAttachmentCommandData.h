#pragma once
#include "CommandDataBase.h"
#include "json.hpp"

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
		{
			//empty
		}
	};
}
