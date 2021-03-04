#pragma once
#include "CommandDataBase.h"
#include "nlohmann/json.hpp"

namespace ravendb::client::documents::commands::batches
{
	class DeleteCommandData : public CommandDataBase
	{
	public:
		DeleteCommandData(std::string id, std::string change_vector)
			: CommandDataBase(std::move(id), {}, std::move(change_vector), CommandType::DELETE_)
		{}

		nlohmann::json serialize() const override;

		virtual void serialize_extra_fields(nlohmann::json& json) const
		{}//empty by design

		void on_before_save_changes(std::shared_ptr<session::InMemoryDocumentSessionOperations> session) override
		{}
	};
}
