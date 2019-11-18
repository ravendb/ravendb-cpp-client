#pragma once
#include "CommandDataBase.h"
#include "nlohmann/json.hpp"

namespace ravendb::client::documents::commands::batches
{
	class PutAttachmentCommandData : public CommandDataBase
	{
	private:
		std::istream& _data;
		const std::optional<std::string> _content_type;

	public:
		PutAttachmentCommandData(std::string document_id, std::string name, std::istream& data,
			std::optional<std::string> content_type, std::string change_vector = "");

		std::istream& get_stream() const
		{
			return _data;
		}

		const std::optional<std::string>& get_content_type() const
		{
			return _content_type;
		}

		nlohmann::json serialize() const override;

		void on_before_save_changes(std::shared_ptr<session::InMemoryDocumentSessionOperations> session) override
		{}
	};
}
