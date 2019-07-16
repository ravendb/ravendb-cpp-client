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

		nlohmann::json serialize() const override;

		void on_before_save_changes(std::shared_ptr<session::InMemoryDocumentSessionOperations> session) override
		{}
	};
}
