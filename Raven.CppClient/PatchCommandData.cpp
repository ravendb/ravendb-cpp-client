#include "stdafx.h"
#include "PatchCommandData.h"
#include "utils.h"
#include "InMemoryDocumentSessionOperations.h"

namespace ravendb::client::documents::commands::batches
{
	PatchCommandData::PatchCommandData(std::string id, std::optional<std::string> change_vector,
		operations::PatchRequest patch, std::optional<operations::PatchRequest> patch_if_missing)
		: CommandDataBase(std::move(id), {}, change_vector ? *change_vector : std::string(),
			CommandType::PATCH)
		, _patch(std::move(patch))
		, _patch_if_missing(std::move(patch_if_missing))
	{}

	const operations::PatchRequest& PatchCommandData::get_patch() const
	{
		return _patch;
	}

	const std::optional<operations::PatchRequest>& PatchCommandData::get_patch_if_missing() const
	{
		return _patch_if_missing;
	}

	nlohmann::json PatchCommandData::serialize() const
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
		set_val_to_json(j, "Type", get_type());
		set_val_to_json(j, "PatchIfMissing", _patch_if_missing, false);

		if (return_document)
		{
			set_val_to_json(j, "ReturnDocument", return_document);
		}

		return j;
	}

	void PatchCommandData::on_before_save_changes(std::shared_ptr<session::InMemoryDocumentSessionOperations> session)
	{
		return_document = session->is_loaded(get_id());
	}
}
