#pragma once
#include "CommandDataBase.h"
#include "json_utils.h"
#include "PatchRequest.h"

namespace ravendb::client::documents::commands::batches
{
	class PatchCommandData : public CommandDataBase
	{
	private:
		const operations::PatchRequest _patch;
		const std::optional<operations::PatchRequest> _patch_if_missing;

	public:
		bool return_document{};

		PatchCommandData(std::string id, std::optional<std::string> change_vector,
			operations::PatchRequest patch, std::optional<operations::PatchRequest> patch_if_missing)
			: CommandDataBase(std::move(id), {}, change_vector ? *change_vector : std::string(), CommandType::PATCH)
			, _patch(std::move(patch))
			, _patch_if_missing(std::move(patch_if_missing))
		{}

		const operations::PatchRequest& get_patch() const
		{
			return _patch;
		}

		const std::optional<operations::PatchRequest>& get_patch_if_missing() const
		{
			return _patch_if_missing;
		}

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
			set_val_to_json(j, "Patch", _patch);
			set_val_to_json(j, "Type", "PATCH");
			set_val_to_json(j, "PatchIfMissing", _patch_if_missing, false);

			if(return_document)
			{
				set_val_to_json(j, "ReturnDocument", return_document);
			}

			return j;
		}

		void on_before_save_changes(std::shared_ptr<session::InMemoryDocumentSessionOperations> session) override
		{
			return_document = session->is_loaded(get_id());
		}
	};
}
